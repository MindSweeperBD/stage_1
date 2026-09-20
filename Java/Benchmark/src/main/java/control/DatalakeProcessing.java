package control;

import model.DatalakeLayout;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashSet;
import java.util.Set;

public class DatalakeProcessing {

    public static Set<String> scanBookIds(DatalakeLayout layout) throws IOException {
        return switch (layout) {
            case TIME_BASED -> scanTimeBased();
            case BOOK_BASED -> scanBookBased();
            case BATCH_BASED -> scanBatchBased();
        };
    }

    private static Set<String> scanTimeBased() throws IOException {
        Set<String> ids = new HashSet<>();

        Files.walk(Path.of("time_datalake"), 3)
                .filter(Files::isRegularFile)
                .forEach(p -> {
                    String name = p.getFileName().toString();
                    String id = name.split("\\.")[0];
                    ids.add(id);
                });
        return ids;
    }

    private static Set<String> scanBookBased() throws IOException {
        Set<String> ids = new HashSet<>();

        Files.list(Path.of("book_datalake"))
                .filter(Files::isDirectory)
                .forEach(dir -> ids.add(dir.getFileName().toString()));
        return ids;
    }

    private static Set<String> scanBatchBased() throws IOException {
        Set<String> ids = new HashSet<>();

        Files.walk(Path.of("batch_datalake"), 2)
                .filter(Files::isRegularFile)
                .forEach(p -> {
                    String name = p.getFileName().toString();
                    if (name.contains(".")) {
                        String id = name.split("\\.")[0];
                        ids.add(id);
                    }
                });
        return ids;
    }
}
