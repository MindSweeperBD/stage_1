#include "BenchmarkRunner.hpp"

#include "model/DatalakeLayout.hpp"

#include <iostream>
#include <vector>

int main() {
    using downloader::benchmark::BenchmarkRunner;
    using downloader::model::DatalakeLayout;

    const std::vector<int> bookIds = {
        1342,
        1610,
        99,
        2700
    };

    const std::vector<DatalakeLayout> layouts = {
        DatalakeLayout::TIME_BASED,
        DatalakeLayout::BOOK_BASED,
        DatalakeLayout::BATCH_BASED
    };

    std::cout
        << "========================================\n"
        << "      DATALAKE BENCHMARK MAIN\n"
        << "========================================\n";

    for (const auto layout : layouts) {
        std::cout
            << "\nLayout: "
            << downloader::benchmark::layoutName(layout)
            << '\n';

        BenchmarkRunner runner(layout);

        runner.benchmarkThroughput(bookIds);

        for (const int bookId : bookIds) {
            runner.benchmarkLookup(
                std::to_string(bookId)
            );
        }

        runner.benchmarkIncrementalProcessing();

        runner.benchmarkRecovery(bookIds);

        runner.benchmarkStorage();
    }

    std::cout
        << "\nDatalake benchmark completed.\n";

    return 0;
}
