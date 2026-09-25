#include "control/TextNormalizer.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <locale>
#include <codecvt>

namespace datamart::control {

std::unordered_set<std::string> TextNormalizer::normalize(
    const std::filesystem::path& bodyPath
) {
    std::ifstream file(bodyPath, std::ios::binary);

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
    std::wstring_convert<
        std::codecvt_utf8<wchar_t>
    > converter;

    std::wstring wideText;

    try {
        wideText = converter.from_bytes(text);
    }
    catch (const std::range_error&) {
        throw std::runtime_error(
            "Invalid UTF-8 text encountered while normalizing."
        );
    }

    std::locale locale("");

    for (wchar_t& character : wideText) {

        if (std::iswalnum(character)) {
            character = std::towlower(character);
        }
        else {
            character = L' ';
        }
    }

    return converter.to_bytes(wideText);
}

}
