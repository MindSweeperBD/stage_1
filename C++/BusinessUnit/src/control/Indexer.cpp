#include "control/Indexer.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace datamart::control {

void Indexer::markListAsIndexed(
    const std::vector<datamart::model::Metadata>& books,
    const std::filesystem::path& baseDirectory
) {
    for (const auto& book : books) {
        markAsIndexed(
            book.bookId,
            baseDirectory
        );
    }
}

void Indexer::markAsIndexed(
    int bookId,
    const std::filesystem::path& baseDirectory
) {
    const std::filesystem::path controlPath =
        baseDirectory / "control";

    const std::filesystem::path indexedBooksPath =
        controlPath / "indexed_books.txt";

    std::filesystem::create_directories(controlPath);

    indexIfNotPresent(
        indexedBooksPath,
        bookId
    );
}

void Indexer::indexIfNotPresent(
    const std::filesystem::path& indexedBooksPath,
    int bookId
) {
    std::unordered_set<int> indexedBooks;

    {
        std::ifstream input(indexedBooksPath);

        int id;

        while (input >> id) {
            indexedBooks.insert(id);
        }
    }

    if (indexedBooks.contains(bookId)) {
        return;
    }

    std::ofstream output(
        indexedBooksPath,
        std::ios::out | std::ios::app
    );

    if (!output.is_open()) {
        throw std::runtime_error(
            "Could not open indexed_books.txt"
        );
    }

    output << bookId << '\n';

    if (!output) {
        throw std::runtime_error(
            "Could not update indexed_books.txt"
        );
    }
}

}
