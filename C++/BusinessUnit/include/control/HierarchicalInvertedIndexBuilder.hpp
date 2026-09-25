#include "control/HierarchicalInvertedIndexBuilder.hpp"
#include "control/TextNormalizer.hpp"

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

        // The normalized terms contain letters and numbers.
        // The first character determines the directory.
        const std::string firstCharacter(1, word.front());

        const std::filesystem::path termDirectory =
            outputDirectory / firstCharacter;

        std::filesystem::create_directories(termDirectory);

        const std::filesystem::path termFile =
            termDirectory / (word + ".txt");

        std::ofstream file(
            termFile,
            std::ios::out | std::ios::trunc
        );

        if (!file.is_open()) {
            throw std::runtime_error(
                "Could not create hierarchical index file: " +
                termFile.string()
            );
        }

        for (const int bookId : bookIds) {
            file << bookId << '\n';
        }

        if (!file) {
            throw std::runtime_error(
                "Error while writing hierarchical index file: " +
                termFile.string()
            );
        }
    }
}

}
