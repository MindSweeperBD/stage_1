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
        << "        DATALAKE BENCHMARK\n"
        << "========================================\n";

    for (const auto layout : layouts) {
        BenchmarkRunner runner(layout);

        std::cout
            << "\n----------------------------------------\n"
            << "Layout: "
            << downloader::benchmark::layoutName(layout)
            << '\n'
            << "----------------------------------------\n";

        std::cout << "\n[1] Throughput benchmark\n";
        runner.benchmarkThroughput(bookIds);

        std::cout << "\n[2] Lookup benchmark\n";
        for (const int bookId : bookIds) {
            runner.benchmarkLookup(
                std::to_string(bookId)
            );
        }

        std::cout
            << "\n[3] Incremental processing benchmark\n";
        runner.benchmarkIncrementalProcessing();

        std::cout << "\n[4] Recovery benchmark\n";
        runner.benchmarkRecovery(bookIds);

        std::cout << "\n[5] Storage benchmark\n";
        runner.benchmarkStorage();
    }

    std::cout
        << "\n========================================\n"
        << "        BENCHMARK FINISHED\n"
        << "========================================\n";

    return 0;
}
