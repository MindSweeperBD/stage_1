#pragma once

#include <string>

namespace datamart::model {

struct Metadata {
    int bookId;
    std::string title;
    std::string author;
    std::string language;
    std::string bodyPath;
};

}
