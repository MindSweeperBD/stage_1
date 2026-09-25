#pragma once

#include "model/Metadata.hpp"

#include <filesystem>
#include <vector>

namespace datamart::control {

class DatalakeReader {
public:
    explicit DatalakeReader(
        const std::filesystem::path& datalakePath
    );

    std::vector<datamart::model::Metadata>
    readMetadata() const;

private:
    std::filesystem::path datalakePath;

    static datamart::model::Metadata getMetadata(
        const std::filesystem::path& headerPath
    );

    static int getBookId(
        const std::filesystem::path& headerPath
    );

    static std::filesystem::path getBodyPath(
        const std::filesystem::path& headerPath,
        int bookId
    );
};

}
