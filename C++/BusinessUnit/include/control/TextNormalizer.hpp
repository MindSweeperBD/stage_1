#pragma once

#include <filesystem>
#include <string>
#include <unordered_set>

namespace datamart::control {

class TextNormalizer {
public:
    static std::unordered_set<std::string> normalize(
        const std::filesystem::path& bodyPath
    );

private:
    static std::string normalizeText(
        const std::string& text
    );
};

}
