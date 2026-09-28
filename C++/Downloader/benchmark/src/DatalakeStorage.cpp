#include "DatalakeStorage.hpp"

#include <filesystem>

namespace downloader::benchmark {

DatalakeStorage::DatalakeStorage(
    downloader::model::DatalakeLayout layout,
    const std::filesystem::path& baseDirectory
)
    : layout_(layout),
      baseDirectory_(baseDirectory) {
}

StorageStats DatalakeStorage::measure() const {
    StorageStats stats;

    const std::filesystem::path datalakePath =
        getDatalakePath();

    if (!std::filesystem::exists(datalakePath)) {
        return stats;
    }

    inspectDirectory(
        datalakePath,
        stats
    );

    const std::filesystem::path controlDirectory =
        baseDirectory_ / "control";

    if (std::filesystem::exists(controlDirectory)) {
        for (const auto& entry :
             std::filesystem::directory_iterator(
                 controlDirectory
             )) {

            if (entry.is_regular_file()) {
                ++stats.auxiliary;
                stats.totalBytes +=
                    entry.file_size();
            }
        }
    }

    return stats;
}

std::filesystem::path
DatalakeStorage::getDatalakePath() const {
    using enum downloader::model::DatalakeLayout;

    switch (layout_) {
        case TIME_BASED:
            return baseDirectory_ /
                   "time_datalake";

        case BOOK_BASED:
            return baseDirectory_ /
                   "book_datalake";

        case BATCH_BASED:
            return baseDirectory_ /
                   "batch_datalake";
    }

    return {};
}

void DatalakeStorage::inspectDirectory(
    const std::filesystem::path& directory,
    StorageStats& stats
) {
    if (!std::filesystem::exists(directory)) {
        return;
    }

    // Count the datalake root directory.
    ++stats.directories;

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(
             directory
         )) {

        if (entry.is_directory()) {
            ++stats.directories;
        }
        else if (entry.is_regular_file()) {
            ++stats.files;

            stats.totalBytes +=
                entry.file_size();
        }
    }
}

}
