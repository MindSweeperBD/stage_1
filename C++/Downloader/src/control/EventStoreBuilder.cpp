#include "control/EventStoreBuilder.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

namespace downloader::control {

namespace {

constexpr int BATCH_SIZE = 1000;

void writeEvent(
    const std::filesystem::path& filePath,
    const downloader::model::BookEvent& event
) {
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(
            filePath.parent_path()
        );
    }

    std::ofstream file(
        filePath,
        std::ios::out | std::ios::trunc | std::ios::binary
    );

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not create datalake file: " +
            filePath.string()
        );
    }

    file << event.getContent();

    if (!file) {
        throw std::runtime_error(
            "Error while writing datalake file: " +
            filePath.string()
        );
    }
}

class TimeBasedEventStore final : public EventStore {
public:
    explicit TimeBasedEventStore(
        std::filesystem::path baseDirectory
    )
        : baseDirectory_(std::move(baseDirectory)) {
    }

    void store(
        const downloader::model::BookEvent& event
    ) override {
        const std::filesystem::path filePath =
            baseDirectory_ /
            "time_datalake" /
            event.getDate() /
            event.getHour() /
            (
                std::to_string(event.getBookId()) +
                "." +
                event.getType() +
                ".txt"
            );

        writeEvent(filePath, event);
    }

private:
    std::filesystem::path baseDirectory_;
};

class BookBasedEventStore final : public EventStore {
public:
    explicit BookBasedEventStore(
        std::filesystem::path baseDirectory
    )
        : baseDirectory_(std::move(baseDirectory)) {
    }

    void store(
        const downloader::model::BookEvent& event
    ) override {
        const std::filesystem::path filePath =
            baseDirectory_ /
            "book_datalake" /
            std::to_string(event.getBookId()) /
            (event.getType() + ".txt");

        writeEvent(filePath, event);
    }

private:
    std::filesystem::path baseDirectory_;
};

class BatchBasedEventStore final : public EventStore {
public:
    explicit BatchBasedEventStore(
        std::filesystem::path baseDirectory
    )
        : baseDirectory_(std::move(baseDirectory)) {
    }

    void store(
        const downloader::model::BookEvent& event
    ) override {
        const int bookId = event.getBookId();

        const int batchStart =
            (bookId / BATCH_SIZE) * BATCH_SIZE;

        const int batchEnd =
            batchStart + BATCH_SIZE - 1;

        const std::string batchDirectory =
            std::to_string(batchStart) +
            "-" +
            std::to_string(batchEnd);

        const std::filesystem::path filePath =
            baseDirectory_ /
            "batch_datalake" /
            batchDirectory /
            (
                std::to_string(bookId) +
                "." +
                event.getType() +
                ".txt"
            );

        writeEvent(filePath, event);
    }

private:
    std::filesystem::path baseDirectory_;
};

}

std::unique_ptr<EventStore> EventStoreBuilder::build(
    downloader::model::DatalakeLayout layout,
    const std::filesystem::path& baseDirectory
) {
    switch (layout) {

        case downloader::model::DatalakeLayout::TIME_BASED:
            return std::make_unique<TimeBasedEventStore>(
                baseDirectory
            );

        case downloader::model::DatalakeLayout::BOOK_BASED:
            return std::make_unique<BookBasedEventStore>(
                baseDirectory
            );

        case downloader::model::DatalakeLayout::BATCH_BASED:
            return std::make_unique<BatchBasedEventStore>(
                baseDirectory
            );
    }

    throw std::invalid_argument(
        "Unsupported datalake layout."
    );
}

}
