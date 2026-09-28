#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <unordered_set>

namespace control {

class ControlLayer {
public:
    explicit ControlLayer(
        std::filesystem::path baseDirectory = "."
    );

    void controlPipelineStep();

private:
    static constexpr int TOTAL_BOOKS = 70000;

    std::filesystem::path baseDirectory_;

    std::filesystem::path controlPath_;
    std::filesystem::path downloadedBooksPath_;
    std::filesystem::path indexedBooksPath_;

    std::filesystem::path datamartPath_;
    std::filesystem::path metadataDatabasePath_;
    std::filesystem::path datalakePath_;
    std::filesystem::path indexPath_;

    std::mt19937 randomGenerator_;

    std::unordered_set<std::string>
    readBookIds(
        const std::filesystem::path& file
    ) const;

    void downloadNewBook(
        const std::unordered_set<std::string>& downloaded
    );

    void downloadBook(
        int bookId
    );

    void indexNextBook(
        const std::unordered_set<std::string>& readyToIndex
    );

    void indexBook(
        const std::string& bookId
    );
};

}
