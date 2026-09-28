#pragma once

#include "StorageStats.hpp"
#include "model/DatalakeLayout.hpp"

#include <filesystem>

namespace downloader::benchmark {

class DatalakeStorage {
public:
    explicit DatalakeStorage(
        downloader::model::DatalakeLayout layout,
        const std::filesystem::path& baseDirectory = "."
    );

    StorageStats measure() const;

private:
    downloader::model::DatalakeLayout layout_;
    std::filesystem::path baseDirectory_;

    std::filesystem::path getDatalakePath() const;

    static void inspectDirectory(
        const std::filesystem::path& directory,
        StorageStats& stats
    );
};

}
