#pragma once

#include "model/DatalakeLayout.hpp"

#include <filesystem>
#include <optional>
#include <string>

namespace downloader::benchmark {

class DatalakeLookup {
public:
    static std::optional<std::filesystem::path> findBookPart(
        downloader::model::DatalakeLayout layout,
        const std::string& bookId,
        const std::string& type,
        const std::filesystem::path& baseDirectory = "."
    );

private:
    static std::optional<std::filesystem::path> findTimeBased(
        const std::string& bookId,
        const std::string& type,
        const std::filesystem::path& baseDirectory
    );

    static std::optional<std::filesystem::path> findBookBased(
        const std::string& bookId,
        const std::string& type,
        const std::filesystem::path& baseDirectory
    );

    static std::optional<std::filesystem::path> findBatchBased(
        const std::string& bookId,
        const std::string& type,
        const std::filesystem::path& baseDirectory
    );
};

}
