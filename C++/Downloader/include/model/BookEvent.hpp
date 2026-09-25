#pragma once

#include <string>
#include <utility>

namespace downloader::model {

class BookEvent {
public:
    BookEvent(
        std::string date,
        std::string hour,
        std::string source,
        int bookId,
        std::string type,
        std::string content
    )
        : date_(std::move(date)),
          hour_(std::move(hour)),
          source_(std::move(source)),
          bookId_(bookId),
          type_(std::move(type)),
          content_(std::move(content)) {
    }

    const std::string& getDate() const {
        return date_;
    }

    const std::string& getHour() const {
        return hour_;
    }

    const std::string& getSource() const {
        return source_;
    }

    int getBookId() const {
        return bookId_;
    }

    const std::string& getType() const {
        return type_;
    }

    const std::string& getContent() const {
        return content_;
    }

private:
    std::string date_;
    std::string hour_;
    std::string source_;
    int bookId_;
    std::string type_;
    std::string content_;
};

}
