#include "control/MetadataDatabase.hpp"

#include <sqlite3.h>

#include <stdexcept>
#include <string>

namespace datamart::control {

MetadataDatabase::MetadataDatabase(
    const std::filesystem::path& databasePath
) : database(nullptr) {

    if (sqlite3_open(databasePath.string().c_str(), &database) != SQLITE_OK) {
        std::string error = sqlite3_errmsg(database);
        sqlite3_close(database);
        database = nullptr;

        throw std::runtime_error(
            "Could not open database: " + error
        );
    }
}

MetadataDatabase::~MetadataDatabase() {
    if (database != nullptr) {
        sqlite3_close(database);
    }
}

void MetadataDatabase::initialize() {

    execute(
        "CREATE TABLE IF NOT EXISTS books ("
        "book_id INTEGER PRIMARY KEY,"
        "title TEXT,"
        "author TEXT,"
        "language TEXT,"
        "body_path TEXT"
        ");"
    );

    execute(
        "CREATE INDEX IF NOT EXISTS idx_books_author "
        "ON books(author);"
    );

    execute(
        "CREATE INDEX IF NOT EXISTS idx_books_title "
        "ON books(title);"
    );

    execute(
        "CREATE INDEX IF NOT EXISTS idx_books_language "
        "ON books(language);"
    );
}

void MetadataDatabase::insertMetadata(
    const std::vector<datamart::model::Metadata>& metadataList
) {
    const char* sql =
        "INSERT OR REPLACE INTO books "
        "(book_id, title, author, language, body_path) "
        "VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr
        ) != SQLITE_OK) {

        throw std::runtime_error(
            "Could not prepare insert statement."
        );
    }

    execute("BEGIN TRANSACTION;");

    for (const auto& metadata : metadataList) {

        sqlite3_bind_int(
            statement,
            1,
            metadata.bookId
        );

        sqlite3_bind_text(
            statement,
            2,
            metadata.title.c_str(),
            -1,
            SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement,
            3,
            metadata.author.c_str(),
            -1,
            SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement,
            4,
            metadata.language.c_str(),
            -1,
            SQLITE_TRANSIENT
        );

        sqlite3_bind_text(
            statement,
            5,
            metadata.bodyPath.c_str(),
            -1,
            SQLITE_TRANSIENT
        );

        if (sqlite3_step(statement) != SQLITE_DONE) {
            sqlite3_finalize(statement);
            execute("ROLLBACK;");

            throw std::runtime_error(
                "Could not insert metadata."
            );
        }

        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);
    }

    sqlite3_finalize(statement);

    execute("COMMIT;");
}

void MetadataDatabase::execute(const char* sql) {

    char* errorMessage = nullptr;

    if (sqlite3_exec(
            database,
            sql,
            nullptr,
            nullptr,
            &errorMessage
        ) != SQLITE_OK) {

        std::string error =
            errorMessage != nullptr
                ? errorMessage
                : "Unknown SQLite error";

        sqlite3_free(errorMessage);

        throw std::runtime_error(error);
    }
}

}
