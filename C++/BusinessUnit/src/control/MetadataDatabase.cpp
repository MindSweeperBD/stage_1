#include "control/MetadataDatabase.hpp"

#include <sqlite3.h>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace datamart::control {

MetadataDatabase::MetadataDatabase(
    const std::filesystem::path& databasePath
) : database(nullptr) {

    if (databasePath.has_parent_path()) {
        std::filesystem::create_directories(
            databasePath.parent_path()
        );
    }

    const int result = sqlite3_open(
        databasePath.string().c_str(),
        &database
    );

    if (result != SQLITE_OK) {
        const std::string error =
            database != nullptr
                ? sqlite3_errmsg(database)
                : "Unknown SQLite error";

        if (database != nullptr) {
            sqlite3_close(database);
            database = nullptr;
        }

        throw std::runtime_error(
            "Could not open SQLite database: " + error
        );
    }
}

MetadataDatabase::~MetadataDatabase() {
    if (database != nullptr) {
        sqlite3_close(database);
        database = nullptr;
    }
}

void MetadataDatabase::initialize() {

    execute(
        "CREATE TABLE IF NOT EXISTS books ("
        "book_id INTEGER PRIMARY KEY, "
        "title TEXT NOT NULL, "
        "author TEXT NOT NULL, "
        "language TEXT NOT NULL, "
        "body_path TEXT NOT NULL"
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
    if (metadataList.empty()) {
        return;
    }

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
            "Could not prepare SQLite statement: " +
            std::string(sqlite3_errmsg(database))
        );
    }

    try {
        execute("BEGIN TRANSACTION;");

        for (const auto& metadata : metadataList) {

            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            if (sqlite3_bind_int(
                    statement,
                    1,
                    metadata.bookId
                ) != SQLITE_OK) {

                throw std::runtime_error(
                    "Could not bind book ID: " +
                    std::string(sqlite3_errmsg(database))
                );
            }

            if (sqlite3_bind_text(
                    statement,
                    2,
                    metadata.title.c_str(),
                    -1,
                    SQLITE_TRANSIENT
                ) != SQLITE_OK) {

                throw std::runtime_error(
                    "Could not bind title: " +
                    std::string(sqlite3_errmsg(database))
                );
            }

            if (sqlite3_bind_text(
                    statement,
                    3,
                    metadata.author.c_str(),
                    -1,
                    SQLITE_TRANSIENT
                ) != SQLITE_OK) {

                throw std::runtime_error(
                    "Could not bind author: " +
                    std::string(sqlite3_errmsg(database))
                );
            }

            if (sqlite3_bind_text(
                    statement,
                    4,
                    metadata.language.c_str(),
                    -1,
                    SQLITE_TRANSIENT
                ) != SQLITE_OK) {

                throw std::runtime_error(
                    "Could not bind language: " +
                    std::string(sqlite3_errmsg(database))
                );
            }

            if (sqlite3_bind_text(
                    statement,
                    5,
                    metadata.bodyPath.c_str(),
                    -1,
                    SQLITE_TRANSIENT
                ) != SQLITE_OK) {

                throw std::runtime_error(
                    "Could not bind body path: " +
                    std::string(sqlite3_errmsg(database))
                );
            }

            if (sqlite3_step(statement) != SQLITE_DONE) {
                throw std::runtime_error(
                    "Could not insert metadata for book " +
                    std::to_string(metadata.bookId) +
                    ": " +
                    sqlite3_errmsg(database)
                );
            }
        }

        execute("COMMIT;");
    }
    catch (...) {
        sqlite3_exec(
            database,
            "ROLLBACK;",
            nullptr,
            nullptr,
            nullptr
        );

        sqlite3_finalize(statement);
        throw;
    }

    sqlite3_finalize(statement);
}

std::vector<datamart::model::Metadata>
MetadataDatabase::findByAuthor(
    const std::string& author
) {
    const char* sql =
        "SELECT book_id, title, author, language, body_path "
        "FROM books "
        "WHERE author = ? "
        "ORDER BY book_id;";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr
        ) != SQLITE_OK) {

        throw std::runtime_error(
            "Could not prepare findByAuthor statement: " +
            std::string(sqlite3_errmsg(database))
        );
    }

    if (sqlite3_bind_text(
            statement,
            1,
            author.c_str(),
            -1,
            SQLITE_TRANSIENT
        ) != SQLITE_OK) {

        const std::string error =
            sqlite3_errmsg(database);

        sqlite3_finalize(statement);

        throw std::runtime_error(
            "Could not bind author parameter: " +
            error
        );
    }

    std::vector<datamart::model::Metadata> results;

    int result;

    while (
        (result = sqlite3_step(statement))
        == SQLITE_ROW
    ) {
        datamart::model::Metadata metadata;

        metadata.bookId =
            sqlite3_column_int(
                statement,
                0
            );

        const auto* title =
            sqlite3_column_text(
                statement,
                1
            );

        const auto* authorValue =
            sqlite3_column_text(
                statement,
                2
            );

        const auto* language =
            sqlite3_column_text(
                statement,
                3
            );

        const auto* bodyPath =
            sqlite3_column_text(
                statement,
                4
            );

        metadata.title =
            title != nullptr
                ? reinterpret_cast<const char*>(title)
                : "";

        metadata.author =
            authorValue != nullptr
                ? reinterpret_cast<const char*>(authorValue)
                : "";

        metadata.language =
            language != nullptr
                ? reinterpret_cast<const char*>(language)
                : "";

        metadata.bodyPath =
            bodyPath != nullptr
                ? reinterpret_cast<const char*>(bodyPath)
                : "";

        results.push_back(
            std::move(metadata)
        );
    }

    if (result != SQLITE_DONE) {
        const std::string error =
            sqlite3_errmsg(database);

        sqlite3_finalize(statement);

        throw std::runtime_error(
            "Could not execute findByAuthor query: " +
            error
        );
    }

    sqlite3_finalize(statement);

    return results;
}

std::optional<std::string>
MetadataDatabase::findBodyPathById(
    int bookId
) {
    const char* sql =
        "SELECT body_path "
        "FROM books "
        "WHERE book_id = ?;";

    sqlite3_stmt* statement = nullptr;

    if (sqlite3_prepare_v2(
            database,
            sql,
            -1,
            &statement,
            nullptr
        ) != SQLITE_OK) {

        throw std::runtime_error(
            "Could not prepare findBodyPathById statement: " +
            std::string(sqlite3_errmsg(database))
        );
    }

    if (sqlite3_bind_int(
            statement,
            1,
            bookId
        ) != SQLITE_OK) {

        const std::string error =
            sqlite3_errmsg(database);

        sqlite3_finalize(statement);

        throw std::runtime_error(
            "Could not bind book ID parameter: " +
            error
        );
    }

    const int result =
        sqlite3_step(statement);

    if (result == SQLITE_ROW) {

        const auto* value =
            sqlite3_column_text(
                statement,
                0
            );

        std::optional<std::string> bodyPath;

        if (value != nullptr) {
            bodyPath =
                reinterpret_cast<const char*>(
                    value
                );
        }

        sqlite3_finalize(statement);

        return bodyPath;
    }

    if (result != SQLITE_DONE) {
        const std::string error =
            sqlite3_errmsg(database);

        sqlite3_finalize(statement);

        throw std::runtime_error(
            "Could not execute findBodyPathById query: " +
            error
        );
    }

    sqlite3_finalize(statement);

    return std::nullopt;
}

void MetadataDatabase::execute(
    const char* sql
) {
    char* errorMessage = nullptr;

    const int result = sqlite3_exec(
        database,
        sql,
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK) {

        const std::string error =
            errorMessage != nullptr
                ? errorMessage
                : sqlite3_errmsg(database);

        if (errorMessage != nullptr) {
            sqlite3_free(errorMessage);
        }

        throw std::runtime_error(
            "SQLite error: " + error
        );
    }
}

}
