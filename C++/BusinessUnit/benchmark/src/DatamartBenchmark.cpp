#include "control/MetadataDatabase.hpp"
#include "model/Metadata.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

std::vector<datamart::model::Metadata>
generateMetadata(std::size_t count) {
    std::vector<datamart::model::Metadata> metadataList;
    metadataList.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        const int bookId =
            static_cast<int>(i + 1);

        datamart::model::Metadata metadata;

        metadata.bookId = bookId;
        metadata.title =
            "Book " + std::to_string(bookId);

        metadata.author =
            "Author " + std::to_string(bookId % 100);

        metadata.language = "English";

        metadata.bodyPath =
            "books/" +
            std::to_string(bookId) +
            ".body.txt";

        metadataList.push_back(
            std::move(metadata)
        );
    }

    return metadataList;
}

double elapsedMilliseconds(
    const Clock::time_point& start,
    const Clock::time_point& end
) {
    return std::chrono::duration<
        double,
        std::milli
    >(end - start).count();
}

void runBenchmark(std::size_t numberOfBooks) {
    const std::filesystem::path databasePath =
        "benchmark_metadata.db";

    if (std::filesystem::exists(databasePath)) {
        std::filesystem::remove(databasePath);
    }

    datamart::control::MetadataDatabase database(
        databasePath
    );

    database.initialize();

    const auto metadataList =
        generateMetadata(numberOfBooks);

    // -----------------------------
    // INSERT BENCHMARK
    // -----------------------------

    const auto insertStart =
        Clock::now();

    database.insertMetadata(
        metadataList
    );

    const auto insertEnd =
        Clock::now();

    // -----------------------------
    // FIND BY AUTHOR BENCHMARK
    // -----------------------------

    const auto authorStart =
        Clock::now();

    const auto booksByAuthor =
        database.findByAuthor(
            "Author 99"
        );

    const auto authorEnd =
        Clock::now();

    // -----------------------------
    // FIND BY ID BENCHMARK
    // -----------------------------

    const int searchId =
        static_cast<int>(
            numberOfBooks / 2
        );

    const auto idStart =
        Clock::now();

    const auto bodyPath =
        database.findBodyPathById(
            searchId
        );

    const auto idEnd =
        Clock::now();

    // -----------------------------
    // RESULTS
    // -----------------------------

    std::cout
        << "\nDataset size: "
        << numberOfBooks
        << " books\n";

    std::cout
        << "Insert metadata: "
        << elapsedMilliseconds(
               insertStart,
               insertEnd
           )
        << " ms\n";

    std::cout
        << "Find by author: "
        << elapsedMilliseconds(
               authorStart,
               authorEnd
           )
        << " ms\n";

    std::cout
        << "Author 99 -> "
        << booksByAuthor.size()
        << " results\n";

    std::cout
        << "Find by ID: "
        << elapsedMilliseconds(
               idStart,
               idEnd
           )
        << " ms\n";

    std::cout
        << "Book "
        << searchId
        << " -> ";

    if (bodyPath.has_value()) {
        std::cout
            << bodyPath.value()
            << '\n';
    }
    else {
        std::cout
            << "not found\n";
    }

    if (std::filesystem::exists(databasePath)) {
        std::filesystem::remove(databasePath);
    }
}

}

int main() {
    const std::vector<std::size_t> datasetSizes = {
        100,
        1000,
        5000,
        10000,
        25000,
        50000
    };

    std::cout
        << "========================================\n"
        << "        DATAMART BENCHMARK\n"
        << "========================================\n";

    for (const auto size : datasetSizes) {
        runBenchmark(size);
    }

    std::cout
        << "\n========================================\n"
        << "        BENCHMARK FINISHED\n"
        << "========================================\n";

    return 0;
}
