#pragma once

#include "model/DatalakeLayout.hpp"

#include <chrono>
#include <filesystem>
#include <vector>

namespace downloader::control {

class BookFeeder {
public:
    static std::vector<std::chrono::system_clock::time_point>
    saveBooks(
        const std::vector<int>& bookIds,
        const std::vector<downloader::model::DatalakeLayout>& layouts,
        const std::filesystem::path& baseDirectory = "."
    );
};

}
