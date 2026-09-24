package datamart;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.sql.SQLException;
import java.util.List;

public class DatamartMain {

    private static final Logger log = LoggerFactory.getLogger(DatamartMain.class);

    private static final Path datalakePath = Path.of("batch_datalake");;
    private static final Path datamartPath = Path.of("datamart");
    private static final Path metadataDatabase = datamartPath.resolve("metadata.db");
    private static final Path invertedIndexFile = datamartPath.resolve("inverted_index.json");

    private static final DatalakeReader reader = new DatalakeReader(datalakePath);
    private static final MetadataDatabase database = new MetadataDatabase(metadataDatabase.toString());
    private static final InvertedIndexBuilder indexBuilder = new InvertedIndexBuilder();

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
        manageInvertedIndex(books);

        log.info("Datamart built correctly");
    }

    private static List<Metadata> readDatalake() throws IOException {
        log.trace("Reading datalake...");
        List<Metadata> books = reader.readMetadata();

        log.trace("Found books: {}", books.size());
        return books;
    }

    private static void manageDatabase(List<Metadata> books) throws SQLException {
        log.trace("Creating the metadata database...");
        database.createDatabase();

        database.insertMetadata(books);
        log.trace("datamart.Metadata saved in: {}", metadataDatabase);
    }

    private static void manageInvertedIndex(List<Metadata> books) throws IOException {
        log.trace("Building inverted index...");
        indexBuilder.processBooks(books);

        indexBuilder.saveInvertedIndex(invertedIndexFile);
        log.trace("Inverted index saved in: {}", invertedIndexFile);
        log.trace("Number of Terms: {}", indexBuilder.getNumberOfTerms());
    }
}
