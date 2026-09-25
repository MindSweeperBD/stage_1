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
            "Could not open header file: " + headerPath.string()
        );
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    const std::string content = buffer.str();

    const std::string title =
        extractField(content, "Title");

    const std::string author =
        extractField(content, "Author");

    const std::string language =
        extractField(content, "Language");

    return {
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
        "^" + field + R"(:\s*(.*)$)",
        std::regex_constants::icase |
        std::regex_constants::multiline
    );

    std::smatch match;

    if (std::regex_search(content, match, pattern)) {
        return match[1].str();
    }

    return "";
}

}
