#pragma once

#include "model/DatalakeLayout.hpp"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace downloader::benchmark {

class DatalakeProcessing {
public:
    static std::unordered_set<std::string> scanBookIds(
        downloader::model::DatalakeLayout layout,
        const std::filesystem::path& baseDirectory = "."
    );

private:
    static std::unordered_set<std::string> scanTimeBased(
        const std::filesystem::path& baseDirectory
    );

    static std::unordered_set<std::string> scanBookBased(
        const std::filesystem::path& baseDirectory
    );

    static std::unordered_set<std::string> scanBatchBased(
        const std::filesystem::path& baseDirectory
    );

    static void extractBookIdFromFile(
        const std::filesystem::path& file,
        std::unordered_set<std::string>& bookIds
    );
};

}
