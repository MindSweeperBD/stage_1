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
createDataset(std::size_t size) {
    std::vector<datamart::model::Metadata> metadataList;
    metadataList.reserve(size);

    for (std::size_t i = 0; i < size; ++i) {
        const int bookId =
            static_cast<int>(i + 1);

        datamart::model::Metadata metadata;

        metadata.bookId = bookId;

        metadata.title =
            "Book " +
            std::to_string(bookId);

        metadata.author =
            "Author " +
            std::to_string(bookId % 100);

        metadata.language =
            "English";

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

double toMilliseconds(
    const Clock::duration& duration
) {
    return std::chrono::duration<
        double,
        std::milli
    >(duration).count();
}

void executeBenchmark(
    std::size_t datasetSize
) {
    const std::filesystem::path databasePath =
        "datamart_benchmark.db";

    if (std::filesystem::exists(databasePath)) {
        std::filesystem::remove(databasePath);
    }

    datamart::control::MetadataDatabase database(
        databasePath
    );

    database.initialize();

    const auto metadata =
        createDataset(datasetSize);

    std::cout
        << "\n----------------------------------------\n"
        << "Dataset: "
        << datasetSize
        << " books\n"
        << "----------------------------------------\n";

    // INSERT

    const auto insertStart =
        Clock::now();

    database.insertMetadata(metadata);

    const auto insertEnd =
        Clock::now();

    std::cout
        << "Insert: "
        << toMilliseconds(
               insertEnd - insertStart
           )
        << " ms\n";

    // SEARCH BY AUTHOR

    const auto authorStart =
        Clock::now();

    const auto authorResults =
        database.findByAuthor(
            "Author 99"
        );

    const auto authorEnd =
        Clock::now();

    std::cout
        << "Author lookup: "
        << toMilliseconds(
               authorEnd - authorStart
           )
        << " ms\n";

    std::cout
        << "Author 99 results: "
        << authorResults.size()
        << '\n';

    // SEARCH BY BOOK ID

    const int targetId =
        static_cast<int>(
            datasetSize / 2
        );

    const auto idStart =
        Clock::now();

    const auto bodyPath =
        database.findBodyPathById(
            targetId
        );

    const auto idEnd =
        Clock::now();

    std::cout
        << "ID lookup: "
        << toMilliseconds(
               idEnd - idStart
           )
        << " ms\n";

    std::cout
        << "Book "
        << targetId
        << ": ";

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
    const std::vector<std::size_t> sizes = {
        100,
        1000,
        5000,
        10000,
        25000,
        50000
    };

    std::cout
        << "========================================\n"
        << "      DATAMART BENCHMARK MAIN\n"
        << "========================================\n";

    for (const auto size : sizes) {
        executeBenchmark(size);
    }

    std::cout
        << "\nDatamart benchmark completed.\n";

    return 0;
}
