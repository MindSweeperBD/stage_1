package control;

import model.BookEvent;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

import model.DatalakeLayout;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class EventStoreBuilder implements EventStore {

    private final String baseDir = "datalake";
    private final DatalakeLayout layout;

    private static final Logger log = LoggerFactory.getLogger(EventStoreBuilder.class);

    public EventStoreBuilder(DatalakeLayout layout){
        this.layout = layout;
    }

    @Override
    public void store(BookEvent book) {
        try {
            File file = resolveEventFile(book.getBookId(), book.getType(), book.getDate(), book.getHour());
            writeEventToFile(file, book.getContent());
        } catch (Exception e) {
            log.error("Error handling event: {}", e.getMessage());
        }
    }

    private File resolveEventFile(int id, String type, String date, String hour) {
        return switch (layout) {
            case TIME_BASED -> resolveTimeBased(id, type, date, hour);
            case BOOK_BASED -> resolveBookBased(id, type);
            case BATCH_BASED -> resolveBatchBased(id, type);
        };
    }

    private File resolveTimeBased(int id, String type, String date, String hour) {
        File path = new File(
                "time_" + baseDir + File.separator +
                        date + File.separator +
                        hour
        );
        if (!path.exists() && !path.mkdirs()) {
            log.warn("Warning: could not create directory {}", path.getAbsolutePath());
        }
        return new File(path, id + "." + type + ".txt");
    }

    private File resolveBookBased(int id, String type) {
        File path = new File(
                "book_" + baseDir + File.separator +
                        id
        );
        if (!path.exists() && !path.mkdirs()) {
            log.warn("Warning: could not create directory {}", path.getAbsolutePath());
        }
        return new File(path, type + ".txt");
    }

    private File resolveBatchBased(int id, String type) {
        String batchFolder = getBatchRange(id);
        File path = new File(
                "batch_" + baseDir + File.separator +
                        batchFolder
        );
        if (!path.exists() && !path.mkdirs()) {
            log.warn("Warning: could not create directory {}", path.getAbsolutePath());
        }
        return new File(path, id + "." + type + ".txt");
    }

    private static String getBatchRange(int id) {
        int batchSize = 1000;

        int batchStart = (id / batchSize) * batchSize;
        int batchEnd = batchStart + batchSize - 1;
        return batchStart + "-" + batchEnd;
    }

    private void writeEventToFile(File file, String content) {
        try (FileWriter fw = new FileWriter(file, true)) {
            fw.write(content);
            fw.write("\n");
            log.trace("Event stored in: {}", file.getAbsolutePath());
        } catch (IOException e) {
            log.error("Error writing event to file: {}", e.getMessage());
        }
    }
}
