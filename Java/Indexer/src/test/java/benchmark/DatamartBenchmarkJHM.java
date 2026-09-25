package benchmark;

import datamart.model.Metadata;
import datamart.control.MetadataDatabase;
import org.openjdk.jmh.annotations.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.MILLISECONDS)
@State(Scope.Benchmark)
public class DatamartBenchmarkJHM {

    private static final Logger log = LoggerFactory.getLogger(DatamartBenchmarkJHM.class);

    private MetadataDatabase database;
    private List<Metadata> metadata;

    @Param({"100", "1000", "5000", "10000", "25000", "50000"})
    private int numberOfBooks;

    @Setup(Level.Trial)
    public void setup() throws Exception {
        createDatabase("datamart_benchmark");
        createMetadata();
    }

    private Path createDatabase(String datamartPath) throws SQLException, IOException {
        Path databasePath = Files.createTempFile(datamartPath, ".db");
        database = new MetadataDatabase(databasePath.toString());
        database.createDatabase();
        return databasePath;
    }

    private void createMetadata() throws SQLException {
        metadata = new ArrayList<>();
        for (int i = 1; i <= numberOfBooks; i++) {
            metadata.add(
                    new Metadata(
                            i,
                            "Book " + i,
                            "Author " + (i % 100),
                            "English",
                            "books/" + i + ".body.txt"
                    )
            );
        }
        database.insertMetadata(metadata);
    }

    @Benchmark
    public void insertionSpeed() throws Exception {
        Path databasePath = createDatabase("insertion_benchmark");
        database.insertMetadata(metadata);
        Files.deleteIfExists(databasePath);
    }

    @Benchmark
    public void queryByAuthor() throws Exception {
        List<Metadata> authorMetadata = database.findByAuthor("Author 99");
        log.info("Books found for author Jane Austen: {}", authorMetadata);
    }

    @Benchmark
    public void queryById() throws Exception {
        int bookId = numberOfBooks / 2;
        String bodyPath = database.findBodyPathById(bookId);
        log.info("Body path for book {}: {}", bookId, bodyPath);
    }
}
