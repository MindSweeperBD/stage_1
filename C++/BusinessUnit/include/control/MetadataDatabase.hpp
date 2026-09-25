#pragma once

#include "model/Metadata.hpp"

#include <filesystem>
#include <vector>

struct sqlite3;

namespace datamart::control {

class MetadataDatabase {
public:
    explicit MetadataDatabase(
        const std::filesystem::path& databasePath
    );

    ~MetadataDatabase();

    MetadataDatabase(const MetadataDatabase&) = delete;
    MetadataDatabase& operator=(const MetadataDatabase&) = delete;

    void initialize();

    void insertMetadata(
        const std::vector<datamart::model::Metadata>& metadataList
    );

private:
    sqlite3* database;

    void execute(const char* sql);
};

}
