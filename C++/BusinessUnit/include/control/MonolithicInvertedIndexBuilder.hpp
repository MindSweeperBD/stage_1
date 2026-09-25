#pragma once

#include "model/Metadata.hpp"

#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace datamart::control {

class MonolithicInvertedIndexBuilder {
public:
    static void build(
        const std::vector<datamart::model::Metadata>& metadataList,
        const std::filesystem::path& outputPath
    );

private:
    static std::map<std::string, std::set<int>> buildIndex(
        const std::vector<datamart::model::Metadata>& metadataList
    );

    static void writeJson(
        const std::map<std::string, std::set<int>>& index,
        const std::filesystem::path& outputPath
    );
};

}
