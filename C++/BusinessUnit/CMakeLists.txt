cmake_minimum_required(VERSION 3.16)

project(BusinessUnit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc i18n)

find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
    pkg_check_modules(
        MONGOCXX
        QUIET
        libmongocxx
    )

    pkg_check_modules(
        BSONCXX
        QUIET
        libbsoncxx
    )
endif()

set(
    BUSINESSUNIT_SOURCES

    src/control/DatalakeReader.cpp
    src/control/HeaderParser.cpp
    src/control/MetadataDatabase.cpp
    src/control/TextNormalizer.cpp
    src/control/MonolithicInvertedIndexBuilder.cpp
    src/control/HierarchicalInvertedIndexBuilder.cpp
    src/control/Indexer.cpp
)

if(MONGOCXX_FOUND AND BSONCXX_FOUND)
    list(
        APPEND
        BUSINESSUNIT_SOURCES
        src/control/MongoInvertedIndexBuilder.cpp
    )
endif()

add_library(
    BusinessUnitCore
    ${BUSINESSUNIT_SOURCES}
)

target_include_directories(
    BusinessUnitCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    BusinessUnitCore
    PUBLIC
        SQLite::SQLite3
        ICU::uc
        ICU::i18n
)

if(MONGOCXX_FOUND AND BSONCXX_FOUND)

    target_include_directories(
        BusinessUnitCore
        PUBLIC
            ${MONGOCXX_INCLUDE_DIRS}
            ${BSONCXX_INCLUDE_DIRS}
    )

    target_link_directories(
        BusinessUnitCore
        PUBLIC
            ${MONGOCXX_LIBRARY_DIRS}
            ${BSONCXX_LIBRARY_DIRS}
    )

    target_link_libraries(
        BusinessUnitCore
        PUBLIC
            ${MONGOCXX_LIBRARIES}
            ${BSONCXX_LIBRARIES}
    )

    target_compile_definitions(
        BusinessUnitCore
        PUBLIC
            MINDSWEEPER_HAS_MONGODB=1
    )

else()

    message(
        STATUS
        "MongoDB C++ driver not found: MongoDB index output will be skipped"
    )

endif()

add_executable(
    BusinessUnit
    src/DatamartMain.cpp
)

target_link_libraries(
    BusinessUnit
    PRIVATE
        BusinessUnitCore
)

add_executable(
    DatamartBenchmark
    benchmark/src/DatamartBenchmark.cpp
)

target_link_libraries(
    DatamartBenchmark
    PRIVATE
        BusinessUnitCore
)

add_executable(
    DatamartBenchmarkMain
    benchmark/src/DatamartBenchmarkMain.cpp
)

target_link_libraries(
    DatamartBenchmarkMain
    PRIVATE
        BusinessUnitCore
)
