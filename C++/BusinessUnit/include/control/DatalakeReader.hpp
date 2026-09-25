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

    if (!std::filesystem::is_directory(datalakePath)) {
        throw std::runtime_error(
            "Datalake path is not a directory: " +
            datalakePath.string()
        );
    }

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(datalakePath)) {

        if (!entry.is_regular_file()) {
            continue;
        }

        const std::filesystem::path& path = entry.path();
        const std::string filename = path.filename().string();

        constexpr const char* suffix = ".header.txt";
        constexpr std::size_t suffixLength = 11;

        if (filename.size() <= suffixLength ||
            filename.compare(
                filename.size() - suffixLength,
                suffixLength,
                suffix
            ) != 0) {
            continue;
        }

        try {
            metadataList.push_back(getMetadata(path));
        }
        catch (const std::exception& exception) {
            std::cerr
                << "Could not process "
                << path
                << ": "
                << exception.what()
                << '\n';
        }
    }

    return metadataList;
}

datamart::model::Metadata
DatalakeReader::getMetadata(
    const std::filesystem::path& headerPath
) {
    const int bookId = getBookId(headerPath);

    const std::filesystem::path bodyPath =
        getBodyPath(headerPath, bookId);

    if (!std::filesystem::exists(bodyPath)) {
        throw std::runtime_error(
            "Body file does not exist: " +
            bodyPath.string()
        );
    }

    return HeaderParser::parse(
        headerPath,
        bodyPath,
        bookId
    );
}

int DatalakeReader::getBookId(
    const std::filesystem::path& headerPath
) {
    const std::string filename =
        headerPath.filename().string();

    constexpr const char* suffix = ".header.txt";
    constexpr std::size_t suffixLength = 11;

    if (filename.size() <= suffixLength) {
        throw std::runtime_error(
            "Invalid header filename: " + filename
        );
    }

    const std::string idText =
        filename.substr(
            0,
            filename.size() - suffixLength
        );

    std::size_t processedCharacters = 0;
    int bookId;

    try {
        bookId = std::stoi(
            idText,
            &processedCharacters
        );
    }
    catch (const std::exception&) {
        throw std::runtime_error(
            "Invalid book ID in filename: " +
            filename
        );
    }

    if (processedCharacters != idText.size()) {
        throw std::runtime_error(
            "Invalid book ID in filename: " +
            filename
        );
    }

    return bookId;
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
