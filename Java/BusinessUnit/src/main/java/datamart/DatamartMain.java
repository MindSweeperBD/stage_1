package datamart;

import datamart.control.*;
import datamart.model.Metadata;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.sql.SQLException;
import java.util.List;

public class DatamartMain {

    private static final Logger log = LoggerFactory.getLogger(DatamartMain.class);

    private static final Path datalakePath = Path.of("batch_datalake");
    private static final Path datamartPath = Path.of("datamart");
    private static final Path metadataDatabase = datamartPath.resolve("metadata.db");
    private static final Path monolithicIndexDirectory = datamartPath.resolve("inverted_index.json");
    private static final Path hierarchicalIndexDirectory = datamartPath.resolve("inverted_index");

    private static final DatalakeReader reader = new DatalakeReader(datalakePath);
    private static final MetadataDatabase database = new MetadataDatabase(metadataDatabase.toString());

    private static final MonolithicInvertedIndexBuilder monolithicIndexBuilder =
            new MonolithicInvertedIndexBuilder();
    private static final HierarchicalInvertedIndexBuilder hierarchicalIndexBuilder =
            new HierarchicalInvertedIndexBuilder();

    public static void main(String[] args) {
        try {
            buildDatamart();
        } catch (Exception e) {
            log.error("Error building datamart", e);
        }
    }

    private static void buildDatamart() throws IOException, SQLException {
        List<Metadata> books = readDatalake();

        Files.createDirectories(datamartPath);
        manageDatabase(books);

        manageInvertedIndexes(books);

        log.info("Datamart built correctly");
    }

    private static List<Metadata> readDatalake() throws IOException {
        log.info("Reading datalake...");
        List<Metadata> books = reader.readMetadata();

        log.info("Found books: {}", books.size());
        return books;
    }

    private static void manageDatabase(List<Metadata> books) throws SQLException {
        log.info("Creating the metadata database...");
        database.createDatabase();

        database.insertMetadata(books);
        log.info("datamart.model.Metadata saved in: {}", metadataDatabase);
    }

    private static void manageInvertedIndexes(List<Metadata> books) throws IOException {
        log.info("Building inverted indexes...");
        manageMonolithicInvertedIndex(books);
        manageMongoInvertedIndex(books);
        manageHierarchicalInvertedIndex(books);
    }

    private static void manageMonolithicInvertedIndex(List<Metadata> books) throws IOException {
        monolithicIndexBuilder.processBooks(books);

        monolithicIndexBuilder.saveInvertedIndex(monolithicIndexDirectory);
        log.info("Inverted indexes saved in: {}", monolithicIndexDirectory);
        log.info("Number of Terms: {}", monolithicIndexBuilder.getNumberOfTerms());
    }

    private static void manageMongoInvertedIndex(List<Metadata> books) throws IOException {
        MongoInvertedIndexBuilder mongoIndexBuilder =
                new MongoInvertedIndexBuilder(
                        "mongodb://localhost:27017"
                );

        try {
            mongoIndexBuilder.processBooks(books);
        } finally {
            mongoIndexBuilder.close();
        }
    }

    private static void manageHierarchicalInvertedIndex(List<Metadata> books) throws IOException {
        hierarchicalIndexBuilder.processBooks(books);

        hierarchicalIndexBuilder.save(hierarchicalIndexDirectory);
        log.info("Inverted indexes saved in: {}", hierarchicalIndexDirectory);
        log.info("Number of Terms: {}", hierarchicalIndexBuilder.getNumberOfTerms());
    }
}
