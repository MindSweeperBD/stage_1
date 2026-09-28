#pragma once

#include "model/DatalakeLayout.hpp"

#include <chrono>
#include <filesystem>
#include <vector>

namespace downloader::benchmark {

class DatalakeRecovery {
public:
    DatalakeRecovery(
        downloader::model::DatalakeLayout layout,
        const std::vector<std::chrono::system_clock::time_point>& timestamps,
        const std::vector<int>& bookIds,
        const std::filesystem::path& baseDirectory = "."
    );

    bool verifyRecovery() const;

private:
    downloader::model::DatalakeLayout layout_;

    std::vector<std::chrono::system_clock::time_point>
        timestamps_;

    std::vector<int> bookIds_;

    std::filesystem::path baseDirectory_;

    bool verifyBook(
        int bookId
    ) const;
};

}
