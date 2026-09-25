package control;

import model.Book;
import model.BookEvent;
import model.DatalakeLayout;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.StandardOpenOption;
import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static control.BookDownloader.downloadBook;

public class BookFeeder {

    private static final File downloadedFile = new File("control/downloaded_books.txt");
    private static final Logger log = LoggerFactory.getLogger(BookFeeder.class);

    public static Map<Integer, String[]> saveBooks(List<Integer> bookIds, DatalakeLayout layout) throws IOException, InterruptedException {
        EventStoreBuilder builder = new EventStoreBuilder(layout);
        Map<Integer, String[]> timestamps = new HashMap<>();

        for (int bookId : bookIds) {
            List<BookEvent> bookEvents = getBookEvents(bookId);
            assert bookEvents != null;
            for (BookEvent event: bookEvents) {
                builder.store(event);
                addTimestamp(bookId, timestamps);
            }
            markAsDownloaded(bookId);
        }
        return timestamps;
    }

    private static void addTimestamp(int bookId, Map<Integer, String[]> timestamps) {
        long ts = System.currentTimeMillis();
        String date = getDateFormat("yyyyMMdd", ts);
        String hour = getDateFormat("HH", ts);

        timestamps.put(bookId, new String[]{date, hour});
    }

    public static void markAsDownloaded(int bookId) {
        try {
            List<String> lines = getDownloadedContent();

            if (!lines.contains(String.valueOf(bookId))) {
                Files.writeString(
                        downloadedFile.toPath(),
                        bookId + "\n",
                        StandardOpenOption.APPEND
                );
                log.trace("Book {} marked as downloaded.", bookId);
            }

        } catch (IOException e) {
            log.error("Error updating downloaded_books.txt: {}", e.getMessage());
        }
    }

    private static List<String> getDownloadedContent() throws IOException {
        if (!downloadedFile.exists()) {
            Files.createDirectories(downloadedFile.toPath().getParent());
            Files.createFile(downloadedFile.toPath());
        }
        return Files.readAllLines(downloadedFile.toPath());
    }

    protected static List<BookEvent> getBookEvents(int bookId) throws IOException, InterruptedException {
        Book book = downloadBook(bookId);

        if (book == null) {
            return null;
        }
        return List.of(createHeaderEvent(bookId, book), createBodyEvent(bookId, book));
    }

    private static BookEvent createHeaderEvent(int bookId, Book book) {
        long ts = System.currentTimeMillis();

        return new BookEvent(
                getDateFormat("yyyyMMdd", ts),
                getDateFormat("HH", ts),
                "BookFeeder",
                bookId,
                "header",
                book.header()
        );
    }

    private static BookEvent createBodyEvent(int bookId, Book book) {
        long ts = System.currentTimeMillis();

        return new BookEvent(
                getDateFormat("yyyyMMdd", ts),
                getDateFormat("HH", ts),
                "BookFeeder",
                bookId,
                "body",
                book.body()
        );
    }

    public static String getDateFormat(String format, long ts) {
        return DateTimeFormatter.ofPattern(format)
                .withZone(ZoneId.systemDefault())
                .format(Instant.ofEpochMilli(ts));
    }
}

