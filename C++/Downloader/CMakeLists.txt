cmake_minimum_required(VERSION 3.16)

project(Downloader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCURL REQUIRED libcurl)

# ============================================================
# Downloader core library
# ============================================================

add_library(DownloaderCore
    src/control/BookDownloader.cpp
    src/control/BookFeeder.cpp
    src/control/EventStoreBuilder.cpp
)

target_include_directories(DownloaderCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${LIBCURL_INCLUDE_DIRS}
)

target_link_directories(DownloaderCore
    PUBLIC
        ${LIBCURL_LIBRARY_DIRS}
)

target_link_libraries(DownloaderCore
    PUBLIC
        ${LIBCURL_LIBRARIES}
)

target_compile_options(DownloaderCore
    PRIVATE
        ${LIBCURL_CFLAGS_OTHER}
)

# ============================================================
# Main Downloader executable
# ============================================================

add_executable(Downloader
    src/DownloaderMain.cpp
)

target_link_libraries(Downloader
    PRIVATE
        DownloaderCore
)

# ============================================================
# Benchmark support library
# ============================================================

add_library(DatalakeBenchmarkCore
    benchmark/src/BenchmarkRunner.cpp
    benchmark/src/DatalakeLookup.cpp
    benchmark/src/DatalakeProcessing.cpp
    benchmark/src/DatalakeRecovery.cpp
    benchmark/src/DatalakeStorage.cpp
)

target_include_directories(DatalakeBenchmarkCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/include
)

target_link_libraries(DatalakeBenchmarkCore
    PUBLIC
        DownloaderCore
)

# ============================================================
# Datalake Benchmark
# Equivalent to the Java benchmark/JMH execution
# ============================================================

add_executable(DatalakeBenchmark
    benchmark/src/DatalakeBenchmarkMain.cpp
)

target_link_libraries(DatalakeBenchmark
    PRIVATE
        DatalakeBenchmarkCore
)

# ============================================================
# Standalone Benchmark
# ============================================================

add_executable(DownloaderBenchmark
    benchmark/src/Benchmark.cpp
)

target_link_libraries(DownloaderBenchmark
    PRIVATE
        DatalakeBenchmarkCore
)
