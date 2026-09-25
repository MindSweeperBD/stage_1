#include "control/TextNormalizer.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <unicode/uchar.h>
#include <unicode/unistr.h>
#include <unicode/locid.h>

namespace datamart::control {

std::unordered_set<std::string> TextNormalizer::normalize(
    const std::filesystem::path& bodyPath
) {
    std::ifstream file(
        bodyPath,
        std::ios::binary
    );

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not open body file: " +
            bodyPath.string()
        );
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    if (file.bad()) {
        throw std::runtime_error(
            "Error while reading body file: " +
            bodyPath.string()
        );
    }

    const std::string normalizedText =
        normalizeText(buffer.str());

    std::unordered_set<std::string> words;

    std::istringstream stream(normalizedText);
    std::string word;

    while (stream >> word) {
        icu::UnicodeString unicodeWord =
            icu::UnicodeString::fromUTF8(word);

        if (unicodeWord.countChar32() > 1) {
            words.insert(word);
        }
    }

    return words;
}

std::string TextNormalizer::normalizeText(
    const std::string& text
) {
    icu::UnicodeString unicodeText =
        icu::UnicodeString::fromUTF8(text);

    unicodeText.toLower(icu::Locale::getRoot());

    icu::UnicodeString normalized;

    for (int32_t index = 0;
         index < unicodeText.length();) {

        const UChar32 character =
            unicodeText.char32At(index);

        if (u_isalpha(character) ||
            u_isdigit(character)) {

            normalized.append(character);
        }
        else {
            normalized.append(
                static_cast<UChar32>(' ')
            );
        }

        index += U16_LENGTH(character);
    }

    std::string result;
    normalized.toUTF8String(result);

    return result;
}

}
