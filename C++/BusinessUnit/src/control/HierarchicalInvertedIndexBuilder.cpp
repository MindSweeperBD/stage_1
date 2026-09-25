#include "control/HierarchicalInvertedIndexBuilder.hpp"
#include "control/TextNormalizer.hpp"

#include <fstream>
#include <stdexcept>

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
            index[word].insert(metadata.bookId);
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

        const std::string firstLetter(1, word.front());

        const std::filesystem::path letterDirectory =
            outputDirectory / firstLetter;

        std::filesystem::create_directories(letterDirectory);

        const std::filesystem::path filePath =
            letterDirectory / (word + ".txt");

        std::ofstream file(filePath);

        if (!file.is_open()) {
            throw std::runtime_error(
                "Could not create index file: " +
                filePath.string()
            );
        }

        for (const int bookId : bookIds) {
            file << bookId << '\n';
        }
    }
}

}
