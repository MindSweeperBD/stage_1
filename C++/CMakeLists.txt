cmake_minimum_required(VERSION 3.16)

project(MindSweeperBD LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Downloader)
add_subdirectory(BusinessUnit)

add_executable(
    Control

    Control/src/ControlMain.cpp
    Control/src/control/ControlLayer.cpp
)

target_include_directories(
    Control
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Control/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Downloader/include
        ${CMAKE_CURRENT_SOURCE_DIR}/BusinessUnit/include
)

target_link_libraries(
    Control
    PRIVATE
        DownloaderCore
        BusinessUnitCore
)
