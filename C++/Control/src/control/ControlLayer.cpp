#include "control/ControlLayer.hpp"

#include "control/BookDownloader.hpp"
#include "control/EventStoreBuilder.hpp"
#include "control/Indexer.hpp"
#include "control/DatalakeReader.hpp"
#include "control/MetadataDatabase.hpp"
#include "control/MonolithicInvertedIndexBuilder.hpp"

#include "model/BookEvent.hpp"
#include "model/DatalakeLayout.hpp"
#include "model/Metadata.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace control {

namespace {

std::string formatTime(
    const std::chrono::system_clock::time_point& timePoint,
    const char* format
) {
    const std::time_t time =
        std::chrono::system_clock::to_time_t(
            timePoint
        );

    std::tm localTime{};

#ifdef _WIN32
    localtime_s(
        &localTime,
        &time
    );
#else
    localtime_r(
        &time,
        &localTime
    );
#endif

    std::ostringstream stream;

    stream
        << std::put_time(
               &localTime,
               format
           );

    return stream.str();
}

void markAsDownloaded(
    const std::filesystem::path& filePath,
    int bookId
) {
    std::filesystem::create_directories(
        filePath.parent_path()
    );

    std::unordered_set<int> downloaded;

    {
        std::ifstream input(
            filePath
        );

        int id;

        while (input >> id) {
            downloaded.insert(id);
        }
    }

    if (downloaded.contains(bookId)) {
        return;
    }

    std::ofstream output(
        filePath,
        std::ios::out |
        std::ios::app
    );

    if (!output.is_open()) {
        throw std::runtime_error(
            "Could not open downloaded_books.txt"
        );
    }

    output
        << bookId
        << '\n';
}

}

ControlLayer::ControlLayer(
    std::filesystem::path baseDirectory
)
    : baseDirectory_(
          std::move(baseDirectory)
      ),
      controlPath_(
          baseDirectory_ /
          "control"
      ),
      downloadedBooksPath_(
          controlPath_ /
          "downloaded_books.txt"
      ),
      indexedBooksPath_(
          controlPath_ /
          "indexed_books.txt"
      ),
      datamartPath_(
          baseDirectory_ /
          "datamart"
      ),
      metadataDatabasePath_(
          datamartPath_ /
          "metadata.db"
      ),
      datalakePath_(
          baseDirectory_ /
          "batch_datalake"
      ),
      indexPath_(
          datamartPath_ /
          "inverted_index.json"
      ),
      randomGenerator_(
          std::random_device{}()
      )
{
    std::filesystem::create_directories(
        controlPath_
    );

    std::filesystem::create_directories(
        datamartPath_
    );
}

std::unordered_set<std::string>
ControlLayer::readBookIds(
    const std::filesystem::path& file
) const {
    std::unordered_set<std::string> result;

    std::ifstream input(file);

    if (!input.is_open()) {
        return result;
    }

    std::string id;

    while (input >> id) {
        result.insert(id);
    }

    return result;
}

void ControlLayer::controlPipelineStep() {

    const auto downloaded =
        readBookIds(
            downloadedBooksPath_
        );

    const auto indexed =
        readBookIds(
            indexedBooksPath_
        );

    std::unordered_set<std::string>
        readyToIndex;

    for (const auto& id : downloaded) {
        if (!indexed.contains(id)) {
            readyToIndex.insert(id);
        }
    }

    if (readyToIndex.empty()) {
        downloadNewBook(downloaded);
    }
    else {
        indexNextBook(readyToIndex);
    }
}

void ControlLayer::downloadNewBook(
    const std::unordered_set<std::string>& downloaded
) {
    std::uniform_int_distribution<int>
        distribution(
            1,
            TOTAL_BOOKS
        );

    for (
        int attempt = 0;
        attempt < 100;
        ++attempt
    ) {
        const int candidateId =
            distribution(
                randomGenerator_
            );

        if (
            !downloaded.contains(
                std::to_string(
                    candidateId
                )
            )
        ) {
            std::cout
                << "[CONTROL] Downloading book "
                << candidateId
                << "...\n";

            downloadBook(
                candidateId
            );

            return;
        }
    }

    std::cout
        << "[CONTROL] Could not find a new book.\n";
}

void ControlLayer::downloadBook(
    int bookId
) {
    const auto book =
        downloader::control::
            BookDownloader::download(
                bookId
            );

    if (!book.has_value()) {
        std::cerr
            << "[CONTROL] No events found for book "
            << bookId
            << '\n';

        return;
    }

    const auto timestamp =
        std::chrono::system_clock::now();

    const std::string date =
        formatTime(
            timestamp,
            "%Y%m%d"
        );

    const std::string hour =
        formatTime(
            timestamp,
            "%H"
        );

    downloader::model::BookEvent
        headerEvent(
            date,
            hour,
            "BookFeeder",
            bookId,
            "header",
            book->header
        );

    downloader::model::BookEvent
        bodyEvent(
            date,
            hour,
            "BookFeeder",
            bookId,
            "body",
            book->body
        );

    auto store =
        downloader::control::
            EventStoreBuilder::build(
                downloader::model::
                    DatalakeLayout::BATCH_BASED,
                baseDirectory_
            );

    store->store(
        headerEvent
    );

    store->store(
        bodyEvent
    );

    markAsDownloaded(
        downloadedBooksPath_,
        bookId
    );

    std::cout
        << "[CONTROL] Book "
        << bookId
        << " successfully downloaded.\n";
}

void ControlLayer::indexNextBook(
    const std::unordered_set<std::string>& readyToIndex
) {
    const std::string bookId =
        *readyToIndex.begin();

    std::cout
        << "[CONTROL] Indexing book "
        << bookId
        << "...\n";

    indexBook(
        bookId
    );

    std::cout
        << "[CONTROL] Book "
        << bookId
        << " successfully indexed.\n";
}

void ControlLayer::indexBook(
    const std::string& bookId
) {
    datamart::control::DatalakeReader reader(
        datalakePath_
    );

    const auto metadataList =
        reader.readMetadata();

    const int id =
        std::stoi(bookId);

    const datamart::model::Metadata*
        selectedBook = nullptr;

    for (
        const auto& metadata :
        metadataList
    ) {
        if (metadata.bookId == id) {
            selectedBook =
                &metadata;

            break;
        }
    }

    if (selectedBook == nullptr) {
        throw std::runtime_error(
            "Metadata not found for book: " +
            bookId
        );
    }

    datamart::control::MetadataDatabase database(
        metadataDatabasePath_.string()
    );

    database.initialize();

    database.insertMetadata(
        {*selectedBook}
    );

    datamart::control::
        MonolithicInvertedIndexBuilder::build(
            metadataList,
            indexPath_
        );

    datamart::control::Indexer::
        markAsIndexed(
            id,
            baseDirectory_
        );
}

}
