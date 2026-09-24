package control;

import model.DatalakeLayout;
import model.StorageStats;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.concurrent.atomic.AtomicInteger;

public class DatalakeStorage {

    private static AtomicInteger dirs = new AtomicInteger();
    private static AtomicInteger files = new AtomicInteger();
    private static AtomicInteger aux = new AtomicInteger();
    private final DatalakeLayout layout;

    public DatalakeStorage(DatalakeLayout layout) {
        dirs = new AtomicInteger();
        files = new AtomicInteger();
        aux = new AtomicInteger();
        this.layout = layout;
    }

    public StorageStats measure() throws IOException {

        switch (layout) {
            case TIME_BASED -> countElements(Path.of("time_datalake"));
            case BOOK_BASED -> countElements(Path.of("book_datalake"));
            case BATCH_BASED -> countElements(Path.of("batch_datalake"));
        }
        return new StorageStats(dirs.get(), files.get(), aux.get());
    }

    private void countElements(Path root) throws IOException {
        Files.walk(root)
                .forEach(p -> {
                    if (Files.isDirectory(p)) {
                        dirs.incrementAndGet();
                    } else {
                        files.incrementAndGet();
                        isAuxiliary(p);
                    }
                });
    }

    private void isAuxiliary(Path p) {
        String name = p.getFileName().toString();
        if (name.endsWith(".idx") ||
                name.endsWith(".meta") ||
                name.endsWith(".checkpoint") ||
                name.endsWith(".log")) {
            aux.incrementAndGet();
        }
    }
}

