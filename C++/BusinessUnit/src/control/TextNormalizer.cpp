#include "control/TextNormalizer.hpp"

#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace datamart::control {

std::unordered_set<std::string> TextNormalizer::normalize(
    const std::filesystem::path& bodyPath
) {
    std::ifstream file(bodyPath);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not open body file: " + bodyPath.string()
        );
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    const std::string normalizedText =
        normalizeText(buffer.str());

    std::unordered_set<std::string> words;

    std::istringstream stream(normalizedText);
    std::string word;

    while (stream >> word) {
        if (word.length() > 1) {
            words.insert(word);
        }
    }

    return words;
}

std::string TextNormalizer::normalizeText(
    const std::string& text
) {
    std::string result;
    result.reserve(text.size());

    for (unsigned char character : text) {

        if (std::isalnum(character)) {
            result += static_cast<char>(
                std::tolower(character)
            );
        } else {
            result += ' ';
        }
    }

    return result;
}

}
