#include "BenchmarkRunner.hpp"

#include "DatalakeLookup.hpp"
#include "DatalakeProcessing.hpp"
#include "DatalakeRecovery.hpp"
#include "DatalakeStorage.hpp"
#include "control/BookFeeder.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_set>

namespace downloader::benchmark {

const char* layoutName(
    downloader::model::DatalakeLayout layout
) {
    using enum downloader::model::DatalakeLayout;

    if (layout == TIME_BASED) {
        return "TIME_BASED";
    }

    if (layout == BOOK_BASED) {
        return "BOOK_BASED";
    }

    return "BATCH_BASED";
}

BenchmarkRunner::BenchmarkRunner(
    downloader::model::DatalakeLayout layout,
    const std::filesystem::path& baseDirectory
)
    : layout_(layout),
      baseDirectory_(baseDirectory) {
}

void BenchmarkRunner::benchmarkThroughput(
    const std::vector<int>& bookIds
) const {
    const auto start =
        std::chrono::steady_clock::now();

    const auto timestamps =
        downloader::control::BookFeeder::saveBooks(
            bookIds,
            {layout_},
            baseDirectory_
        );

    const double seconds =
        std::chrono::duration<double>(
            std::chrono::steady_clock::now() - start
        ).count();

    std::cout
        << layoutName(layout_)
        << " layout: "
        << bookIds.size()
        << " books in "
        << seconds
        << " s ("
        << (seconds > 0.0
                ? bookIds.size() / seconds
                : 0.0)
        << " books/s), successful="
        << timestamps.size()
        << '\n';
}

void BenchmarkRunner::benchmarkLookup(
    const std::string& bookId
) const {
    const auto start =
        std::chrono::steady_clock::now();

    const auto header =
        DatalakeLookup::findBookPart(
            layout_,
            bookId,
            "header",
            baseDirectory_
        );

    const auto body =
        DatalakeLookup::findBookPart(
            layout_,
            bookId,
            "body",
            baseDirectory_
        );

    const double milliseconds =
        std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - start
        ).count();

    std::cout
        << layoutName(layout_)
        << " layout: book "
        << bookId
        << " found in "
        << milliseconds
        << " ms (header="
        << static_cast<bool>(header)
        << ", body="
        << static_cast<bool>(body)
        << ")\n";
}

void BenchmarkRunner::benchmarkIncrementalProcessing() const {
    const auto start =
        std::chrono::steady_clock::now();

    std::unordered_set<std::string> indexedBooks;

    std::ifstream file(
        baseDirectory_ /
        "control" /
        "indexed_books.txt"
    );

    for (std::string bookId; file >> bookId;) {
        indexedBooks.insert(bookId);
    }

    auto presentBooks =
        DatalakeProcessing::scanBookIds(
            layout_,
            baseDirectory_
        );

    for (const auto& bookId : indexedBooks) {
        presentBooks.erase(bookId);
    }

    const double milliseconds =
        std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - start
        ).count();

    std::cout
        << layoutName(layout_)
        << " layout: "
        << presentBooks.size()
        << " new books detected in "
        << milliseconds
        << " ms\n";
}

void BenchmarkRunner::benchmarkRecovery(
    const std::vector<int>& bookIds
) const {
    const auto half =
        bookIds.size() / 2;

    const std::vector<int> partialBookIds(
        bookIds.begin(),
        bookIds.begin() + half
    );

    downloader::control::BookFeeder::saveBooks(
        partialBookIds,
        {layout_},
        baseDirectory_
    );

    const auto timestamps =
        downloader::control::BookFeeder::saveBooks(
            bookIds,
            {layout_},
            baseDirectory_
        );

    const auto start =
        std::chrono::steady_clock::now();

    const bool recoverySuccessful =
        DatalakeRecovery(
            layout_,
            timestamps,
            bookIds,
            baseDirectory_
        ).verifyRecovery();

    const double milliseconds =
        std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - start
        ).count();

    std::cout
        << layoutName(layout_)
        << " layout: recovery "
        << (recoverySuccessful ? "OK" : "FAILED")
        << " in "
        << milliseconds
        << " ms\n";
}

void BenchmarkRunner::benchmarkStorage() const {
    const auto stats =
        DatalakeStorage(
            layout_,
            baseDirectory_
        ).measure();

    std::cout
        << layoutName(layout_)
        << " layout: "
        << stats.directories
        << " dirs, "
        << stats.files
        << " files, "
        << stats.auxiliary
        << " auxiliary\n";
}

}
