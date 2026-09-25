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

    std::ofstream file(
        outputPath,
        std::ios::out | std::ios::trunc
    );

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not create inverted index file: " +
            outputPath.string()
        );
    }

    file << "{\n";

    bool firstTerm = true;

    for (const auto& [term, bookIds] : index) {

        if (!firstTerm) {
            file << ",\n";
        }

        firstTerm = false;

        file << "  \"";

        // Escape characters that have a special meaning in JSON.
        for (const char character : term) {
            switch (character) {
                case '"':
                    file << "\\\"";
                    break;

                case '\\':
                    file << "\\\\";
                    break;

                case '\b':
                    file << "\\b";
                    break;

                case '\f':
                    file << "\\f";
                    break;

                case '\n':
                    file << "\\n";
                    break;

                case '\r':
                    file << "\\r";
                    break;

                case '\t':
                    file << "\\t";
                    break;

                default:
                    file << character;
                    break;
            }
        }

        file << "\": [";

        bool firstBook = true;

        for (const int bookId : bookIds) {

            if (!firstBook) {
                file << ", ";
            }

            firstBook = false;
            file << bookId;
        }

        file << "]";
    }

    file << "\n}\n";

    if (!file) {
        throw std::runtime_error(
            "Error while writing inverted index file: " +
            outputPath.string()
        );
    }
}

}
