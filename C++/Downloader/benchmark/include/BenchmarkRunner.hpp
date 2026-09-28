#pragma once

#include "model/DatalakeLayout.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace downloader::benchmark {

class BenchmarkRunner {
public:
    explicit BenchmarkRunner(
        downloader::model::DatalakeLayout layout,
        const std::filesystem::path& baseDirectory = "."
    );

    void benchmarkThroughput(
        const std::vector<int>& bookIds
    ) const;

    void benchmarkLookup(
        const std::string& bookId
    ) const;

    void benchmarkIncrementalProcessing() const;

    void benchmarkRecovery(
        const std::vector<int>& bookIds
    ) const;

    void benchmarkStorage() const;

private:
    downloader::model::DatalakeLayout layout_;
    std::filesystem::path baseDirectory_;
};

const char* layoutName(
    downloader::model::DatalakeLayout layout
);

}
