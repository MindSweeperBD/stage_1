#include "control/DatalakeReader.hpp"
#include "control/HeaderParser.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace datamart::control {

DatalakeReader::DatalakeReader(
    const std::filesystem::path& datalakePath
) : datalakePath(datalakePath) {
}

std::vector<datamart::model::Metadata>
DatalakeReader::readMetadata() const {

    std::vector<datamart::model::Metadata> metadataList;

    if (!std::filesystem::exists(datalakePath)) {
        throw std::runtime_error(
            "Datalake path does not exist: " +
            datalakePath.string()
        );
    }

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(datalakePath)) {

        if (!entry.is_regular_file()) {
            continue;
        }

        const auto& path = entry.path();
        const std::string fileName =
            path.filename().string();

        if (!fileName.ends_with(".header.txt")) {
            continue;
        }

        try {
            metadataList.push_back(
                getMetadata(path)
            );
        }
        catch (const std::exception& e) {
            std::cerr
                << "Error processing: "
                << path
                << ", "
                << e.what()
                << '\n';
        }
    }

    return metadataList;
}

datamart::model::Metadata
DatalakeReader::getMetadata(
    const std::filesystem::path& headerPath
) {
    const int bookId =
        getBookId(headerPath);

    const auto bodyPath =
        getBodyPath(headerPath, bookId);

    return HeaderParser::parse(
        headerPath,
        bodyPath,
        bookId
    );
}

int DatalakeReader::getBookId(
    const std::filesystem::path& headerPath
) {
    const std::string fileName =
        headerPath.filename().string();

    constexpr const char* suffix = ".header.txt";

    const std::size_t position =
        fileName.find(suffix);

    if (position == std::string::npos) {
        throw std::runtime_error(
            "Invalid header filename: " + fileName
        );
    }

    return std::stoi(
        fileName.substr(0, position)
    );
}

std::filesystem::path
DatalakeReader::getBodyPath(
    const std::filesystem::path& headerPath,
    int bookId
) {
    return headerPath.parent_path() /
           (std::to_string(bookId) + ".body.txt");
}

}
