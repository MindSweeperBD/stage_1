cmake_minimum_required(VERSION 3.20)

project(BusinessUnit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SQLite
find_package(SQLite3 REQUIRED)

# MongoDB C++ Driver
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

add_executable(BusinessUnit
    src/DatamartMain.cpp
    src/control/DatalakeReader.cpp
    src/control/HeaderParser.cpp
    src/control/MetadataDatabase.cpp
    src/control/TextNormalizer.cpp
    src/control/MonolithicInvertedIndexBuilder.cpp
    src/control/HierarchicalInvertedIndexBuilder.cpp
    src/control/MongoInvertedIndexBuilder.cpp
)

target_include_directories(BusinessUnit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(BusinessUnit
    PRIVATE
        SQLite::SQLite3
        mongo::mongocxx_shared
        mongo::bsoncxx_shared
)
