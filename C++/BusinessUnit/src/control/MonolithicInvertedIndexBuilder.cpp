#include "control/MonolithicInvertedIndexBuilder.hpp"
#include "control/TextNormalizer.hpp"

#include <fstream>
#include <stdexcept>

namespace datamart::control {

void MonolithicInvertedIndexBuilder::build(
    const std::vector<datamart::model::Metadata>& metadataList,
    const std::filesystem::path& outputPath
) {
    const auto index = buildIndex(metadataList);

    writeJson(index, outputPath);
}

std::map<std::string, std::set<int>>
MonolithicInvertedIndexBuilder::buildIndex(
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

void MonolithicInvertedIndexBuilder::writeJson(
    const std::map<std::string, std::set<int>>& index,
    const std::filesystem::path& outputPath
) {
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(
            outputPath.parent_path()
        );
    }

    std::ofstream file(outputPath);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not create inverted index file: " +
            outputPath.string()
        );
    }

    file << "{\n";

    auto termIterator = index.begin();

    while (termIterator != index.end()) {

        file << "  \"" << termIterator->first << "\": [";

        auto idIterator = termIterator->second.begin();

        while (idIterator != termIterator->second.end()) {

            file << *idIterator;

            ++idIterator;

            if (idIterator != termIterator->second.end()) {
                file << ", ";
            }
        }

        file << "]";

        ++termIterator;

        if (termIterator != index.end()) {
            file << ",";
        }

        file << "\n";
    }

    file << "}\n";
}

}
