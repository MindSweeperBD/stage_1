#include "control/MongoInvertedIndexBuilder.hpp"
#include "control/TextNormalizer.hpp"

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

#include <map>
#include <set>
#include <string>

namespace datamart::control {

MongoInvertedIndexBuilder::MongoInvertedIndexBuilder(
    const std::string& connectionString
) : connectionString(connectionString) {
}

void MongoInvertedIndexBuilder::build(
    const std::vector<datamart::model::Metadata>& metadataList
) {
    static mongocxx::instance instance{};

    mongocxx::client client{
        mongocxx::uri{connectionString}
    };

    auto database = client["datamart"];
    auto collection = database["inverted_index"];

    std::map<std::string, std::set<int>> index;

    for (const auto& metadata : metadataList) {

        const auto words =
            TextNormalizer::normalize(metadata.bodyPath);

        for (const auto& word : words) {
            index[word].insert(metadata.bookId);
        }
    }

    for (const auto& [word, bookIds] : index) {

        bsoncxx::builder::basic::array ids;

        for (const int bookId : bookIds) {
            ids.append(bookId);
        }

        bsoncxx::builder::basic::document filter;
        filter.append(
            bsoncxx::builder::basic::kvp("term", word)
        );

        bsoncxx::builder::basic::document update;
        update.append(
            bsoncxx::builder::basic::kvp(
                "$addToSet",
                [bookIds](bsoncxx::builder::basic::sub_document subDoc) {
                    subDoc.append(
                        bsoncxx::builder::basic::kvp(
                            "postings",
                            [bookIds](bsoncxx::builder::basic::sub_document eachDoc) {

                                bsoncxx::builder::basic::array values;

                                for (const int id : bookIds) {
                                    values.append(id);
                                }

                                eachDoc.append(
                                    bsoncxx::builder::basic::kvp(
                                        "$each",
                                        values
                                    )
                                );
                            }
                        )
                    );
                }
            )
        );

        mongocxx::options::update options;
        options.upsert(true);

        collection.update_one(
            filter.view(),
            update.view(),
            options
        );
    }

    mongocxx::options::index indexOptions;
    indexOptions.unique(true);

    bsoncxx::builder::basic::document indexDocument;
    indexDocument.append(
        bsoncxx::builder::basic::kvp("term", 1)
    );

    collection.create_index(
        indexDocument.view(),
        indexOptions
    );
}

}
