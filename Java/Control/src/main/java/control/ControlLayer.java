package control;

import datamart.control.DatalakeReader;
import datamart.control.MetadataDatabase;
import datamart.control.MonolithicInvertedIndexBuilder;
import datamart.model.Metadata;
import model.BookEvent;
import model.DatalakeLayout;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.sql.SQLException;
import java.util.*;

import static control.BookFeeder.getBookEvents;
import static datamart.control.Indexer.markAsIndexed;
import static control.BookFeeder.markAsDownloaded;
import static model.DatalakeLayout.BATCH_BASED;

public class ControlLayer {

    private static final Logger log = LoggerFactory.getLogger(ControlLayer.class);

    private static final Path CONTROL_PATH = Path.of("control");
    private static final Path DOWNLOADED_BOOKS = CONTROL_PATH.resolve("downloaded_books.txt");
    private static final Path INDEXED_BOOKS = CONTROL_PATH.resolve("indexed_books.txt");

    private static final int TOTAL_BOOKS = 70000;
    private final Random random = new Random();

    private final DatalakeLayout USED_LAYOUT = BATCH_BASED;

    private static final Path DATAMART_PATH = Path.of("datamart");
    private static final Path metadataDatabase = DATAMART_PATH.resolve("metadata.db");
    private static final MetadataDatabase database = new MetadataDatabase(metadataDatabase.toString());

    private static final Path datalakePath = Path.of("batch_datalake");
    private static final MonolithicInvertedIndexBuilder monolithicIndexBuilder =
            new MonolithicInvertedIndexBuilder();
    public static final Path INDEXPATH = Path.of("datamart/inverted_index.json");

    public ControlLayer() throws IOException {
        Files.createDirectories(CONTROL_PATH);
        Files.createDirectories(DATAMART_PATH);
    }

    public void controlPipelineStep() throws IOException, InterruptedException, SQLException {
        Set<String> downloaded = readBookIds(DOWNLOADED_BOOKS);
        Set<String> indexed = readBookIds(INDEXED_BOOKS);

        Set<String> readyToIndex = new HashSet<>(downloaded);
        readyToIndex.removeAll(indexed);

        if (readyToIndex.isEmpty()) {
            downloadNewBook(downloaded);
        } else {
            indexNextBook(readyToIndex);
        }
    }

    private Set<String> readBookIds(Path file) throws IOException {
        if (!Files.exists(file)) {
            return new HashSet<>();
        }
        return new HashSet<>(
                Files.readAllLines(file, StandardCharsets.UTF_8)
        );
    }

    private void indexNextBook(Set<String> readyToIndex) throws IOException, SQLException {
        String bookId = readyToIndex.iterator().next();
        log.info("[CONTROL] Scheduling book {} for indexing...", bookId);

        indexBook(bookId);
        log.info("[CONTROL] Book {} successfully indexed.", bookId);
    }

    private void downloadNewBook(Set<String> downloaded) throws IOException, InterruptedException {
        for (int i = 0; i < 10; i++) {
            int candidateId = random.nextInt(TOTAL_BOOKS) + 1;

            if (!downloaded.contains(String.valueOf(candidateId))) {
                log.info("[CONTROL] Downloading new book with ID {}...", candidateId);

                downloadBook(candidateId);
                log.info("[CONTROL] Book {} successfully downloaded.", candidateId);

                return;
            }
        }
        log.info("[CONTROL] Could not find a new book.");
    }

    private void downloadBook(int bookId) throws IOException, InterruptedException {
        EventStoreBuilder builder = new EventStoreBuilder(USED_LAYOUT);

        List<BookEvent> bookEvents = getBookEvents(bookId);

        if (bookEvents == null) {
            log.warn("No events found for book {}", bookId);
            return;
        }
        for (BookEvent event : bookEvents) {
            builder.store(event);
        }
        markAsDownloaded(bookId);
    }

    private void indexBook(String bookId) throws IOException, SQLException {
        database.createDatabase();

        Metadata bookMetadata = getMetadata(bookId);
        database.insertMetadata(List.of(bookMetadata));

        monolithicIndexBuilder.processBook(bookMetadata);
        monolithicIndexBuilder.saveInvertedIndex(INDEXPATH);
        markAsIndexed(Integer.parseInt(bookId));
    }

    private static Metadata getMetadata(String bookId) throws IOException {
        DatalakeReader reader = new DatalakeReader(datalakePath);
        List<Metadata> metadataList = reader.readMetadata();

        int id = Integer.parseInt(bookId);

        return metadataList.stream()
                .filter(metadata -> metadata.bookId() == id)
                .findFirst()
                .orElseThrow(() ->
                        new IllegalArgumentException(
                                "Metadata not found for book: " + bookId
                        )
                );
    }
}
