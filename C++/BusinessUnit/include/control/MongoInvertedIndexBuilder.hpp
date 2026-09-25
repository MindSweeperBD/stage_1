#pragma once

#include "model/Metadata.hpp"

#include <string>
#include <vector>

namespace datamart::control {

class MongoInvertedIndexBuilder {
public:
    explicit MongoInvertedIndexBuilder(
        std::string connectionString = "mongodb://localhost:27017"
    );

    void build(
        const std::vector<datamart::model::Metadata>& metadataList
    ) const;

private:
    std::string connectionString;
};

}
