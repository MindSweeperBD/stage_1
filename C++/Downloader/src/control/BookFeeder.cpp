#include "control/BookFeeder.hpp"

#include "control/BookDownloader.hpp"
#include "control/EventStoreBuilder.hpp"
#include "model/BookEvent.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace downloader::control {

namespace {

std::string formatTime(
    const std::chrono::system_clock::time_point& timePoint,
    const char* format
) {
    const std::time_t time =
        std::chrono::system_clock::to_time_t(timePoint);

    std::tm localTime{};

#ifdef _WIN32
    localtime_s(&localTime, &time);
#else
    localtime_r(&time, &localTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&localTime, format);

    return stream.str();
}

void saveIndexedBook(
    const std::filesystem::path& indexPath,
    int bookId
) {
    if (indexPath.has_parent_path()) {
        std::filesystem::create_directories(
            indexPath.parent_path()
        );
    }

    // Avoid storing the same book ID more than once.
    {
        std::ifstream inputFile(indexPath);

        int indexedBookId;

        while (inputFile >> indexedBookId) {
            if (indexedBookId == bookId) {
                return;
            }
        }
    }

    std::ofstream file(
        indexPath,
        std::ios::out | std::ios::app
    );

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not open indexed books file: " +
            indexPath.string()
        );
    }

    file << bookId << '\n';

    if (!file) {
        throw std::runtime_error(
            "Could not update indexed books file: " +
            indexPath.string()
        );
    }
}

}

std::vector<std::chrono::system_clock::time_point>
BookFeeder::saveBooks(
    const std::vector<int>& bookIds,
    const std::vector<downloader::model::DatalakeLayout>& layouts,
    const std::filesystem::path& baseDirectory
) {
    std::vector<std::chrono::system_clock::time_point> timestamps;

    const std::filesystem::path indexPath =
        baseDirectory /
        "control" /
        "indexed_books.txt";

    std::vector<std::unique_ptr<EventStore>> stores;
    stores.reserve(layouts.size());

    for (const auto layout : layouts) {
        stores.push_back(
            EventStoreBuilder::build(
                layout,
                baseDirectory
            )
        );
    }

    for (const int bookId : bookIds) {

        const auto book =
            BookDownloader::download(bookId);

        if (!book.has_value()) {
            std::cerr
                << "Book "
                << bookId
                << " could not be downloaded.\n";

            continue;
        }

        const auto timestamp =
            std::chrono::system_clock::now();

        const std::string date =
            formatTime(timestamp, "%Y%m%d");

        const std::string hour =
            formatTime(timestamp, "%H");

        downloader::model::BookEvent headerEvent(
            date,
            hour,
            "BookFeeder",
            bookId,
            "header",
            book->header
        );

        downloader::model::BookEvent bodyEvent(
            date,
            hour,
            "BookFeeder",
            bookId,
            "body",
            book->body
        );

        for (const auto& store : stores) {
            store->store(headerEvent);
            store->store(bodyEvent);
        }

        saveIndexedBook(
            indexPath,
            bookId
        );

        timestamps.push_back(timestamp);

        std::cout
            << "Book "
            << bookId
            << " stored successfully.\n";
    }

    return timestamps;
}

}
