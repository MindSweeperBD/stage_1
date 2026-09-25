#include "control/MongoInvertedIndexBuilder.hpp"
#include "control/TextNormalizer.hpp"

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/index.hpp>
#include <mongocxx/options/update.hpp>
#include <mongocxx/uri.hpp>

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

namespace datamart::control {

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

MongoInvertedIndexBuilder::MongoInvertedIndexBuilder(
    std::string connectionString
) : connectionString(std::move(connectionString)) {
}

void MongoInvertedIndexBuilder::build(
    const std::vector<datamart::model::Metadata>& metadataList
) const {
    // Only one MongoDB driver instance is required per application.
    static mongocxx::instance instance{};

    mongocxx::client client{
        mongocxx::uri{connectionString}
    };

    auto database = client["datamart"];
    auto collection = database["inverted_index"];

    /*
     * Build the inverted index in memory:
     *
     * term -> {bookId, bookId, ...}
     */
    std::map<std::string, std::set<int>> invertedIndex;

    for (const auto& metadata : metadataList) {

        const auto words =
            TextNormalizer::normalize(metadata.bodyPath);

        for (const auto& word : words) {

            if (!word.empty()) {
                invertedIndex[word].insert(metadata.bookId);
            }
        }
    }

    /*
     * Create a unique index for "term".
     *
     * MongoDB will therefore prevent two documents from
     * representing the same term.
     */
    mongocxx::options::index indexOptions;
    indexOptions.unique(true);

    collection.create_index(
        make_document(
            kvp("term", 1)
        ),
        indexOptions
    );

    /*
     * Insert/update each term.
     *
     * Resulting MongoDB document:
     *
     * {
     *     "term": "book",
     *     "postings": [99, 177, 1342]
     * }
     */
    for (const auto& [term, bookIds] : invertedIndex) {

        bsoncxx::builder::basic::array postings;

        for (const int bookId : bookIds) {
            postings.append(bookId);
        }

        auto filter = make_document(
            kvp("term", term)
        );

        auto update = make_document(
            kvp(
                "$addToSet",
                make_document(
                    kvp(
                        "postings",
                        make_document(
                            kvp(
                                "$each",
                                postings.extract()
                            )
                        )
                    )
                )
            )
        );

        mongocxx::options::update updateOptions;
        updateOptions.upsert(true);

        collection.update_one(
            filter.view(),
            update.view(),
            updateOptions
        );
    }
}

}
