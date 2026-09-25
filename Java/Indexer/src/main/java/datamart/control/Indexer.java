package datamart.control;

import datamart.model.Metadata;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class Indexer {

    private static final Path CONTROL_PATH = Path.of("control");
    private static final Path INDEXED_BOOKS = CONTROL_PATH.resolve("indexed_books.txt");

    public static void markListAsIndexed(List<Metadata> books) throws IOException {
        for (Metadata book : books) {
            markAsIndexed(book.bookId());
        }
    }

    public static void markAsIndexed(int bookId) throws IOException {
        Files.createDirectories(CONTROL_PATH);

        Set<String> indexed = Files.exists(INDEXED_BOOKS)
                ? new HashSet<>(Files.readAllLines(INDEXED_BOOKS))
                : new HashSet<>();

        String id = String.valueOf(bookId);
        indexIfNotPresent(indexed, id);
    }

    private static void indexIfNotPresent(Set<String> indexed, String id) throws IOException {
        if (!indexed.contains(id)) {
            Files.writeString(
                    INDEXED_BOOKS,
                    id + System.lineSeparator(),
                    StandardCharsets.UTF_8,
                    StandardOpenOption.CREATE,
                    StandardOpenOption.APPEND
            );
        }
    }
}
