#pragma once

#include "model/Metadata.hpp"

#include <filesystem>
#include <string>

namespace datamart::control {

class HeaderParser {
public:
    static datamart::model::Metadata parse(
        const std::filesystem::path& headerPath,
        const std::filesystem::path& bodyPath,
        int bookId
    );

private:
    static std::string extractField(
        const std::string& content,
        const std::string& field
    );
};

}
