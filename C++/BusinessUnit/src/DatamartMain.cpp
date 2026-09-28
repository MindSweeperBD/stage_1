#include "control/DatalakeReader.hpp"
#include "control/MetadataDatabase.hpp"
#include "control/MonolithicInvertedIndexBuilder.hpp"
#include "control/HierarchicalInvertedIndexBuilder.hpp"
#include "control/Indexer.hpp"

#ifdef MINDSWEEPER_HAS_MONGODB
#include "control/MongoInvertedIndexBuilder.hpp"
#endif

#include <filesystem>
#include <iostream>

int main() {
    try {
        const std::filesystem::path datalakePath =
            "batch_datalake";

        const std::filesystem::path datamartPath =
            "datamart";

        std::filesystem::create_directories(
            datamartPath
        );

        datamart::control::DatalakeReader reader(
            datalakePath
        );

        const auto metadataList =
            reader.readMetadata();

        std::cout
            << "Books found: "
            << metadataList.size()
            << '\n';

        datamart::control::MetadataDatabase database(
            datamartPath / "metadata.db"
        );

        database.initialize();

        database.insertMetadata(
            metadataList
        );

        datamart::control::
            MonolithicInvertedIndexBuilder::build(
                metadataList,
                datamartPath /
                    "inverted_index.json"
            );

        datamart::control::
            HierarchicalInvertedIndexBuilder::build(
                metadataList,
                datamartPath /
                    "inverted_index"
            );

#ifdef MINDSWEEPER_HAS_MONGODB

        datamart::control::
            MongoInvertedIndexBuilder mongoBuilder;

        mongoBuilder.build(
            metadataList
        );

#else

        std::cout
            << "MongoDB C++ driver not installed; "
            << "MongoDB index skipped.\n";

#endif

        datamart::control::Indexer::
            markListAsIndexed(
                metadataList
            );

        std::cout
            << "Datamart successfully generated.\n";

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr
            << "Error: "
            << e.what()
            << '\n';

        return 1;
    }
}
