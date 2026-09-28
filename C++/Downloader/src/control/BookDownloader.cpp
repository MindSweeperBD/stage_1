#include "control/BookDownloader.hpp"

#include <curl/curl.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

namespace downloader::control {

std::string BookDownloader::buildUrl(
    int bookId
) {
    return
        "https://www.gutenberg.org/cache/epub/" +
        std::to_string(bookId) +
        "/pg" +
        std::to_string(bookId) +
        ".txt";
}

std::size_t BookDownloader::writeCallback(
    void* contents,
    std::size_t size,
    std::size_t nmemb,
    void* userData
) {
    const std::size_t totalSize =
        size * nmemb;

    auto* response =
        static_cast<std::string*>(
            userData
        );

    response->append(
        static_cast<char*>(contents),
        totalSize
    );

    return totalSize;
}

std::optional<downloader::model::Book>
BookDownloader::download(
    int bookId
) {
    static const bool curlInitialized = [] {
        return
            curl_global_init(
                CURL_GLOBAL_DEFAULT
            ) == CURLE_OK;
    }();

    if (!curlInitialized) {
        throw std::runtime_error(
            "Could not initialize libcurl globally."
        );
    }

    const std::string url =
        buildUrl(bookId);

    CURL* curl =
        curl_easy_init();

    if (curl == nullptr) {
        throw std::runtime_error(
            "Could not initialize libcurl."
        );
    }

    std::string response;

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        url.c_str()
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        &BookDownloader::writeCallback
    );

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &response
    );

    curl_easy_setopt(
        curl,
        CURLOPT_FOLLOWLOCATION,
        1L
    );

    curl_easy_setopt(
        curl,
        CURLOPT_USERAGENT,
        "MindSweeper.BD/1.0"
    );

    curl_easy_setopt(
        curl,
        CURLOPT_CONNECTTIMEOUT,
        10L
    );

    curl_easy_setopt(
        curl,
        CURLOPT_TIMEOUT,
        30L
    );

    curl_easy_setopt(
        curl,
        CURLOPT_FAILONERROR,
        1L
    );

    const CURLcode result =
        curl_easy_perform(curl);

    long httpCode = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &httpCode
    );

    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        std::cerr
            << "Could not download book "
            << bookId
            << ". CURL error: "
            << curl_easy_strerror(result)
            << '\n';

        return std::nullopt;
    }

    if (
        httpCode < 200 ||
        httpCode >= 300
    ) {
        std::cerr
            << "Could not download book "
            << bookId
            << ". HTTP status: "
            << httpCode
            << '\n';

        return std::nullopt;
    }

    const std::string startMarker =
        "*** START OF THE PROJECT GUTENBERG EBOOK";

    const std::string endMarker =
        "*** END OF THE PROJECT GUTENBERG EBOOK";

    const std::size_t startPosition =
        response.find(startMarker);

    const std::size_t endPosition =
        response.find(endMarker);

    if (
        startPosition == std::string::npos ||
        endPosition == std::string::npos ||
        endPosition <
            startPosition + startMarker.size()
    ) {
        std::cerr
            << "Gutenberg markers not found for book "
            << bookId
            << '\n';

        return std::nullopt;
    }

    std::string header =
        response.substr(
            0,
            startPosition
        );

    const std::size_t bodyStart =
        startPosition +
        startMarker.size();

    std::string body =
        response.substr(
            bodyStart,
            endPosition - bodyStart
        );

    return downloader::model::Book{
        std::move(header),
        std::move(body)
    };
}

}
