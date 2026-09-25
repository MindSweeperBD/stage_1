#pragma once

#include "model/Book.hpp"

#include <optional>
#include <string>

namespace downloader::control {

class BookDownloader {
public:
    static std::optional<downloader::model::Book>
    download(int bookId);

private:
    static std::string buildUrl(int bookId);

    static std::size_t writeCallback(
        void* contents,
        std::size_t size,
        std::size_t nmemb,
        void* userData
    );
};

}
