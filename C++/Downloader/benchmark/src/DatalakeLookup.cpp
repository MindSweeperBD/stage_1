#include "DatalakeLookup.hpp"

#include <filesystem>
#include <optional>
#include <string>

namespace downloader::benchmark {

std::optional<std::filesystem::path>
DatalakeLookup::findBookPart(
    downloader::model::DatalakeLayout layout,
    const std::string& bookId,
    const std::string& type,
    const std::filesystem::path& baseDirectory
) {
    using enum downloader::model::DatalakeLayout;

    switch (layout) {
        case TIME_BASED:
            return findTimeBased(
                bookId,
                type,
                baseDirectory
            );

        case BOOK_BASED:
            return findBookBased(
                bookId,
                type,
                baseDirectory
            );

        case BATCH_BASED:
            return findBatchBased(
                bookId,
                type,
                baseDirectory
            );
    }

    return std::nullopt;
}

std::optional<std::filesystem::path>
DatalakeLookup::findTimeBased(
    const std::string& bookId,
    const std::string& type,
    const std::filesystem::path& baseDirectory
) {
    const std::filesystem::path datalake =
        baseDirectory / "time_datalake";

    if (!std::filesystem::exists(datalake)) {
        return std::nullopt;
    }

    const std::string expectedFile =
        bookId + "." + type + ".txt";

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(datalake)) {

        if (!entry.is_regular_file()) {
            continue;
        }

        if (entry.path().filename() == expectedFile) {
            return entry.path();
        }
    }

    return std::nullopt;
}

std::optional<std::filesystem::path>
DatalakeLookup::findBookBased(
    const std::string& bookId,
    const std::string& type,
    const std::filesystem::path& baseDirectory
) {
    const std::filesystem::path filePath =
        baseDirectory /
        "book_datalake" /
        bookId /
        (type + ".txt");

    if (std::filesystem::exists(filePath) &&
        std::filesystem::is_regular_file(filePath)) {

        return filePath;
    }

    return std::nullopt;
}

std::optional<std::filesystem::path>
DatalakeLookup::findBatchBased(
    const std::string& bookId,
    const std::string& type,
    const std::filesystem::path& baseDirectory
) {
    int id;

    try {
        id = std::stoi(bookId);
    }
    catch (...) {
        return std::nullopt;
    }

    constexpr int BATCH_SIZE = 1000;

    const int batchStart =
        (id / BATCH_SIZE) * BATCH_SIZE;

    const int batchEnd =
        batchStart + BATCH_SIZE - 1;

    const std::filesystem::path batchDirectory =
        baseDirectory /
        "batch_datalake" /
        (
            std::to_string(batchStart) +
            "-" +
            std::to_string(batchEnd)
        );

    const std::filesystem::path filePath =
        batchDirectory /
        (
            bookId +
            "." +
            type +
            ".txt"
        );

    if (std::filesystem::exists(filePath) &&
        std::filesystem::is_regular_file(filePath)) {

        return filePath;
    }

    return std::nullopt;
}

}
