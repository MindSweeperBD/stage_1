#include "DatalakeProcessing.hpp"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace downloader::benchmark {

std::unordered_set<std::string>
DatalakeProcessing::scanBookIds(
    downloader::model::DatalakeLayout layout,
    const std::filesystem::path& baseDirectory
) {
    using enum downloader::model::DatalakeLayout;

    switch (layout) {
        case TIME_BASED:
            return scanTimeBased(baseDirectory);

        case BOOK_BASED:
            return scanBookBased(baseDirectory);

        case BATCH_BASED:
            return scanBatchBased(baseDirectory);
    }

    return {};
}

std::unordered_set<std::string>
DatalakeProcessing::scanTimeBased(
    const std::filesystem::path& baseDirectory
) {
    std::unordered_set<std::string> bookIds;

    const std::filesystem::path datalake =
        baseDirectory / "time_datalake";

    if (!std::filesystem::exists(datalake)) {
        return bookIds;
    }

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(datalake)) {

        if (!entry.is_regular_file()) {
            continue;
        }

        extractBookIdFromFile(
            entry.path(),
            bookIds
        );
    }

    return bookIds;
}

std::unordered_set<std::string>
DatalakeProcessing::scanBookBased(
    const std::filesystem::path& baseDirectory
) {
    std::unordered_set<std::string> bookIds;

    const std::filesystem::path datalake =
        baseDirectory / "book_datalake";

    if (!std::filesystem::exists(datalake)) {
        return bookIds;
    }

    for (const auto& entry :
         std::filesystem::directory_iterator(datalake)) {

        if (!entry.is_directory()) {
            continue;
        }

        const auto bookId =
            entry.path().filename().string();

        const auto header =
            entry.path() / "header.txt";

        const auto body =
            entry.path() / "body.txt";

        if (std::filesystem::exists(header) ||
            std::filesystem::exists(body)) {

            bookIds.insert(bookId);
        }
    }

    return bookIds;
}

std::unordered_set<std::string>
DatalakeProcessing::scanBatchBased(
    const std::filesystem::path& baseDirectory
) {
    std::unordered_set<std::string> bookIds;

    const std::filesystem::path datalake =
        baseDirectory / "batch_datalake";

    if (!std::filesystem::exists(datalake)) {
        return bookIds;
    }

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator(datalake)) {

        if (!entry.is_regular_file()) {
            continue;
        }

        extractBookIdFromFile(
            entry.path(),
            bookIds
        );
    }

    return bookIds;
}

void DatalakeProcessing::extractBookIdFromFile(
    const std::filesystem::path& file,
    std::unordered_set<std::string>& bookIds
) {
    const std::string filename =
        file.filename().string();

    constexpr const char* headerSuffix =
        ".header.txt";

    constexpr const char* bodySuffix =
        ".body.txt";

    const std::size_t headerPosition =
        filename.find(headerSuffix);

    if (headerPosition != std::string::npos &&
        headerPosition + std::string(headerSuffix).size()
            == filename.size()) {

        bookIds.insert(
            filename.substr(
                0,
                headerPosition
            )
        );

        return;
    }

    const std::size_t bodyPosition =
        filename.find(bodySuffix);

    if (bodyPosition != std::string::npos &&
        bodyPosition + std::string(bodySuffix).size()
            == filename.size()) {

        bookIds.insert(
            filename.substr(
                0,
                bodyPosition
            )
        );
    }
}

}
