package control;

import model.DatalakeLayout;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class DatalakeRecovery {

    private static final Logger log = LoggerFactory.getLogger(DatalakeRecovery.class);

    private final Map<Integer, Integer> headerCount = new HashMap<>();
    private final Map<Integer, Integer> bodyCount = new HashMap<>();
    private final DatalakeLayout layout;
    private final Map<Integer, String[]> timestamps;
    private final List<Integer> bookIds;

    public DatalakeRecovery(DatalakeLayout layout,
                            Map<Integer, String[]> timestamps,
                            List<Integer> bookIds) {
        this.layout = layout;
        this.timestamps = timestamps;
        this.bookIds = bookIds;
        
        for (int id : bookIds) {
            headerCount.put(id, 0);
            bodyCount.put(id, 0);
        }
    }

    public boolean verifyRecovery() throws IOException {
        switch (layout) {
            case TIME_BASED -> verifyTimeBased();
            case BOOK_BASED -> verifyBookBased();
            case BATCH_BASED -> verifyBatchBased();
        }
        return haveABodyAndAHeader();
    }

    private boolean haveABodyAndAHeader() {
        for (int id : bookIds) {
            if (headerCount.get(id) != 1 || bodyCount.get(id) != 1) {
                log.error("TIME_BASED recovery error for book {}: {} {}", id, headerCount.get(id), bodyCount.get(id));
                return false;
            }
        }
        return true;
    }

    private void verifyTimeBased() throws IOException {
        for (int id : bookIds) {
            String[] ts = timestamps.get(id);
            String date = ts[0];
            String hour = ts[1];

            Path folder = Path.of("time_datalake", date, hour);
            if (!Files.exists(folder)) continue;
            searchInTimeFolder(folder, id);
        }
    }

    private void searchInTimeFolder(Path folder, int id) throws IOException {
        Files.list(folder)
                .filter(Files::isRegularFile)
                .forEach(p -> {
                    String name = p.getFileName().toString();
                    String[] parts = name.split("\\.");
                    int fileId = Integer.parseInt(parts[0]);
                    String type = parts[1];

                    if (fileId == id) {
                        isInDatalake(id, type);
                    }
                });
    }

    private void verifyBookBased() throws IOException {
        Files.walk(Path.of("book_datalake"), 3)
                .filter(Files::isRegularFile)
                .forEach(p -> {
                    String name = p.getFileName().toString();
                    String[] parts = name.split("\\.");
                    String type = parts[0];
                    int id = Integer.parseInt(p.getParent().getFileName().toString());

                    isInDatalake(id, type);
                });
    }

    private void verifyBatchBased() throws IOException {
        Files.walk(Path.of("batch_datalake"), 3)
                .filter(Files::isRegularFile)
                .forEach(p -> {
                    String name = p.getFileName().toString();
                    String[] parts = name.split("\\.");
                    int id = Integer.parseInt(parts[0]);
                    String type = parts[1];

                    isInDatalake(id, type);
                });
    }

    private void isInDatalake(int id, String type) {
        if (!headerCount.containsKey(id)) return;
        if (type.equals("header")) headerCount.put(id, headerCount.get(id) + 1);
        else if (type.equals("body")) bodyCount.put(id, bodyCount.get(id) + 1);
    }
}
