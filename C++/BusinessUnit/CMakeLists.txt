cmake_minimum_required(VERSION 3.20)

project(BusinessUnit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -------------------------
# Dependencies
# -------------------------

find_package(SQLite3 REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc)
find_package(bsoncxx REQUIRED)
find_package(mongocxx REQUIRED)

# -------------------------
# BusinessUnit executable
# -------------------------

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

# -------------------------
# Header files
# -------------------------

target_include_directories(BusinessUnit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -------------------------
# Libraries
# -------------------------

target_link_libraries(BusinessUnit
    PRIVATE
        SQLite::SQLite3
        mongo::bsoncxx_shared
        mongo::mongocxx_shared
)
