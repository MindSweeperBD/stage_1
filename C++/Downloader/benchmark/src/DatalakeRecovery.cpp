#include "DatalakeRecovery.hpp"

#include "DatalakeLookup.hpp"

#include <string>

namespace downloader::benchmark {

DatalakeRecovery::DatalakeRecovery(
    downloader::model::DatalakeLayout layout,
    const std::vector<std::chrono::system_clock::time_point>& timestamps,
    const std::vector<int>& bookIds,
    const std::filesystem::path& baseDirectory
)
    : layout_(layout),
      timestamps_(timestamps),
      bookIds_(bookIds),
      baseDirectory_(baseDirectory) {
}

bool DatalakeRecovery::verifyRecovery() const {
    if (bookIds_.empty()) {
        return true;
    }

    for (const int bookId : bookIds_) {
        if (!verifyBook(bookId)) {
            return false;
        }
    }

    return true;
}

bool DatalakeRecovery::verifyBook(
    int bookId
) const {
    const std::string id =
        std::to_string(bookId);

    const auto header =
        DatalakeLookup::findBookPart(
            layout_,
            id,
            "header",
            baseDirectory_
        );

    const auto body =
        DatalakeLookup::findBookPart(
            layout_,
            id,
            "body",
            baseDirectory_
        );

    return header.has_value() &&
           body.has_value();
}

}
