cmake_minimum_required(VERSION 3.16)

project(BusinessUnit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

# ============================================================
# BusinessUnit core library
# ============================================================

add_library(BusinessUnitCore
    src/control/DatalakeReader.cpp
    src/control/HeaderParser.cpp
    src/control/MetadataDatabase.cpp
    src/control/TextNormalizer.cpp
    src/control/MonolithicInvertedIndexBuilder.cpp
    src/control/HierarchicalInvertedIndexBuilder.cpp
)

target_include_directories(BusinessUnitCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(BusinessUnitCore
    PUBLIC
        SQLite::SQLite3
)

# ============================================================
# Main BusinessUnit executable
# ============================================================

add_executable(BusinessUnit
    src/DatamartMain.cpp
)

target_link_libraries(BusinessUnit
    PRIVATE
        BusinessUnitCore
)

# ============================================================
# Datamart benchmark
# ============================================================

add_executable(DatamartBenchmark
    benchmark/src/DatamartBenchmark.cpp
)

target_include_directories(DatamartBenchmark
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(DatamartBenchmark
    PRIVATE
        BusinessUnitCore
)

# ============================================================
# Datamart Benchmark Main
# ============================================================

add_executable(DatamartBenchmarkMain
    benchmark/src/DatamartBenchmarkMain.cpp
)

target_include_directories(DatamartBenchmarkMain
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(DatamartBenchmarkMain
    PRIVATE
        BusinessUnitCore
)
