#pragma once

#include "model/Metadata.hpp"

#include <filesystem>
#include <vector>

namespace datamart::control {

class Indexer {
public:
    static void markListAsIndexed(
        const std::vector<datamart::model::Metadata>& books,
        const std::filesystem::path& baseDirectory = "."
    );

    static void markAsIndexed(
        int bookId,
        const std::filesystem::path& baseDirectory = "."
    );

private:
    static void indexIfNotPresent(
        const std::filesystem::path& indexedBooksPath,
        int bookId
    );
};

}
