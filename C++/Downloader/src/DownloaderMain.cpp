#include "control/BookFeeder.hpp"
#include "model/DatalakeLayout.hpp"

#include <exception>
#include <iostream>
#include <vector>

int main() {
    try {
        const std::vector<downloader::model::DatalakeLayout> layouts{
            downloader::model::DatalakeLayout::TIME_BASED,
            downloader::model::DatalakeLayout::BOOK_BASED,
            downloader::model::DatalakeLayout::BATCH_BASED
        };

        const std::vector<int> bookIds{
            99,
            177,
            1342,
            1610,
            2700
        };

        const auto timestamps =
            downloader::control::BookFeeder::saveBooks(
                bookIds,
                layouts
            );

        std::cout
            << "Downloader finished successfully.\n"
            << "Books processed: "
            << timestamps.size()
            << '\n';

        return 0;
    }
    catch (const std::exception& exception) {
        std::cerr
            << "Downloader error: "
            << exception.what()
            << '\n';

        return 1;
    }
}
