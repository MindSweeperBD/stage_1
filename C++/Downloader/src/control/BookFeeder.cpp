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
    const std::chrono::system_clock::time_point& tp,
    const char* format
) {
    const std::time_t t =
        std::chrono::system_clock::to_time_t(tp);

    std::tm local{};

#ifdef _WIN32
    localtime_s(&local, &t);
#else
    localtime_r(&t, &local);
#endif

    std::ostringstream out;
    out << std::put_time(&local, format);

    return out.str();
}

void markAsDownloaded(
    const std::filesystem::path& path,
    int bookId
) {
    if (path.has_parent_path()) {
        std::filesystem::create_directories(
            path.parent_path()
        );
    }

    {
        std::ifstream in(path);

        int id;

        while (in >> id) {
            if (id == bookId) {
                return;
            }
        }
    }

    std::ofstream out(
        path,
        std::ios::app
    );

    if (!out) {
        throw std::runtime_error(
            "Could not open downloaded books file: " +
            path.string()
        );
    }

    out << bookId << '\n';

    if (!out) {
        throw std::runtime_error(
            "Could not update downloaded books file: " +
            path.string()
        );
    }
}

}

std::vector<std::chrono::system_clock::time_point>
BookFeeder::saveBooks(
    const std::vector<int>& bookIds,
    const std::vector<
        downloader::model::DatalakeLayout
    >& layouts,
    const std::filesystem::path& baseDirectory
) {
    std::vector<
        std::chrono::system_clock::time_point
    > timestamps;

    const auto downloadedPath =
        baseDirectory /
        "control" /
        "downloaded_books.txt";

    std::vector<
        std::unique_ptr<EventStore>
    > stores;

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

        if (!book) {
            std::cerr
                << "Book "
                << bookId
                << " could not be downloaded.\n";

            continue;
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

        markAsDownloaded(
            downloadedPath,
            bookId
        );

        timestamps.push_back(
            timestamp
        );

        std::cout
            << "Book "
            << bookId
            << " stored successfully.\n";
    }

    return timestamps;
}

}
