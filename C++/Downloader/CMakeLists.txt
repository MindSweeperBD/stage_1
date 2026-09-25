cmake_minimum_required(VERSION 3.20)

project(Downloader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------
# Dependencies
# -------------------------

find_package(PkgConfig REQUIRED)

pkg_check_modules(
    LIBCURL
    REQUIRED
    IMPORTED_TARGET
    libcurl
)

# -------------------------
# Downloader executable
# -------------------------

add_executable(Downloader
    src/DownloaderMain.cpp
    src/control/BookDownloader.cpp
    src/control/BookFeeder.cpp
    src/control/EventStoreBuilder.cpp
)

# -------------------------
# Header files
# -------------------------

target_include_directories(Downloader
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -------------------------
# Libraries
# -------------------------

target_link_libraries(Downloader
    PRIVATE
        PkgConfig::LIBCURL
)
