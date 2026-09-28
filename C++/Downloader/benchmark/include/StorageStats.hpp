#pragma once

#include <cstdint>

namespace downloader::benchmark {

struct StorageStats {
    std::uintmax_t directories = 0;
    std::uintmax_t files = 0;
    std::uintmax_t auxiliary = 0;
    std::uintmax_t totalBytes = 0;
};

}
