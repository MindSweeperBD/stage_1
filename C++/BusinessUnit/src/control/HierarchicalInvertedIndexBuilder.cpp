#include "control/HierarchicalInvertedIndexBuilder.hpp"
#include "control/TextNormalizer.hpp"

#include <cctype>
#include <fstream>
#include <stdexcept>
#include <string>

namespace datamart::control {

void HierarchicalInvertedIndexBuilder::build(
    const std::vector<datamart::model::Metadata>& metadataList,
    const std::filesystem::path& outputDirectory
) {
    const auto index = buildIndex(metadataList);
    writeIndex(index, outputDirectory);
}

std::map<std::string, std::set<int>>
HierarchicalInvertedIndexBuilder::buildIndex(
    const std::vector<datamart::model::Metadata>& metadataList
) {
    std::map<std::string, std::set<int>> index;

    for (const auto& metadata : metadataList) {
        const auto words =
            TextNormalizer::normalize(metadata.bodyPath);

        for (const auto& word : words) {
            if (!word.empty()) {
                index[word].insert(metadata.bookId);
            }
        }
    }

    return index;
}

void HierarchicalInvertedIndexBuilder::writeIndex(
    const std::map<std::string, std::set<int>>& index,
    const std::filesystem::path& outputDirectory
) {
    std::filesystem::create_directories(outputDirectory);

    for (const auto& [word, bookIds] : index) {

        if (word.empty()) {
            continue;
        }

        const unsigned char firstCharacter =
            static_cast<unsigned char>(word.front());

        const char upperCharacter =
            static_cast<char>(std::toupper(firstCharacter));

        const std::string firstLetter(1, upperCharacter);

        const std::filesystem::path letterDirectory =
            outputDirectory / firstLetter;

        std::filesystem::create_directories(letterDirectory);

        const std::filesystem::path wordFile =
            letterDirectory / (word + ".txt");

        std::ofstream file(
            wordFile,
            std::ios::out | std::ios::trunc
        );

        if (!file.is_open()) {
            throw std::runtime_error(
                "Could not create index file: " +
                wordFile.string()
            );
        }

        for (const int bookId : bookIds) {
            file << bookId << '\n';
        }

        if (!file) {
            throw std::runtime_error(
                "Error while writing index file: " +
                wordFile.string()
            );
        }
    }
}

}
