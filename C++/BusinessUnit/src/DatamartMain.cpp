#include "control/DatalakeReader.hpp"
#include "control/MetadataDatabase.hpp"
#include "control/MonolithicInvertedIndexBuilder.hpp"
#include "control/HierarchicalInvertedIndexBuilder.hpp"
#include "control/MongoInvertedIndexBuilder.hpp"

#include <filesystem>
#include <iostream>

int main() {

    try {
        const std::filesystem::path datalakePath = "batch_datalake";
        const std::filesystem::path datamartPath = "datamart";

        std::filesystem::create_directories(datamartPath);

        // Read metadata from the batch datalake
        datamart::control::DatalakeReader reader(datalakePath);
        const auto metadataList = reader.readMetadata();

        std::cout << "Books found: "
                  << metadataList.size()
                  << '\n';

        // Store metadata in SQLite
        datamart::control::MetadataDatabase metadataDatabase(
            datamartPath / "metadata.db"
        );

        metadataDatabase.initialize();
        metadataDatabase.insertMetadata(metadataList);

        // Build monolithic inverted index
        datamart::control::MonolithicInvertedIndexBuilder::build(
            metadataList,
            datamartPath / "inverted_index.json"
        );

        // Build hierarchical inverted index
        datamart::control::HierarchicalInvertedIndexBuilder::build(
            metadataList,
            datamartPath / "inverted_index"
        );

        // Build MongoDB inverted index
        datamart::control::MongoInvertedIndexBuilder mongoBuilder;
        mongoBuilder.build(metadataList);

        std::cout << "Datamart successfully generated.\n";

        return 0;

    } catch (const std::exception& exception) {

        std::cerr
            << "Error: "
            << exception.what()
            << '\n';

        return 1;
    }
}
