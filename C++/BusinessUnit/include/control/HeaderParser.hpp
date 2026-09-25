#include "control/HeaderParser.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace datamart::control {

datamart::model::Metadata HeaderParser::parse(
    const std::filesystem::path& headerPath,
    const std::filesystem::path& bodyPath,
    int bookId
) {
    std::ifstream file(headerPath);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not open header file: " +
            headerPath.string()
        );
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    if (file.bad()) {
        throw std::runtime_error(
            "Error while reading header file: " +
            headerPath.string()
        );
    }

    const std::string content = buffer.str();

    const std::string title =
        extractField(content, "Title");

    const std::string author =
        extractField(content, "Author");

    const std::string language =
        extractField(content, "Language");

    return datamart::model::Metadata{
        bookId,
        title,
        author,
        language,
        bodyPath.string()
    };
}

std::string HeaderParser::extractField(
    const std::string& content,
    const std::string& field
) {
    const std::regex pattern(
        "(?:^|\\r?\\n)" +
        field +
        R"(\s*:\s*([^\r\n]*))",
        std::regex_constants::icase
    );

    std::smatch match;

    if (!std::regex_search(content, match, pattern)) {
        return "";
    }

    std::string value = match[1].str();

    const auto first =
        value.find_first_not_of(" \t");

    if (first == std::string::npos) {
        return "";
    }

    const auto last =
        value.find_last_not_of(" \t");

    return value.substr(
        first,
        last - first + 1
    );
}

}
