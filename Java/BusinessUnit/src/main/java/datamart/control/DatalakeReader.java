package datamart.control;

import datamart.model.Metadata;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Stream;

public class DatalakeReader {

    private static final Logger log = LoggerFactory.getLogger(DatalakeReader.class);

    private final Path datalakePath;

    public DatalakeReader(Path datalakePath) {
        this.datalakePath = datalakePath;
    }

    public List<Metadata> readMetadata() throws IOException {
        List<Metadata> metadataList = new ArrayList<>();

        try (Stream<Path> paths = Files.walk(datalakePath)) {
            paths
                    .filter(Files::isRegularFile)
                    .filter(path -> path.getFileName()
                            .toString()
                            .endsWith(".header.txt"))
                    .forEach(headerPath -> {
                        addMetadataToList(headerPath, metadataList);
                    });
        }

        return metadataList;
    }

    private static void addMetadataToList(Path headerPath, List<Metadata> metadataList) {
        try {
            Metadata metadata = getMetadata(headerPath);
            metadataList.add(metadata);
        } catch (Exception e) {
            log.error("Error processing: {}, {}", headerPath, e.getMessage());
        }
    }

    private static Metadata getMetadata(Path headerPath) throws IOException {
        int bookId = getBookId(headerPath);
        Path bodyPath = getBodyPath(headerPath, bookId);

        return HeaderParser.parse(
                headerPath,
                bodyPath,
                bookId
        );
    }

    private static int getBookId(Path headerPath) {
        String fileName =
                headerPath.getFileName().toString();
        String idText = fileName
                .substring(
                        0,
                        fileName.indexOf(".header.txt")
                );

        return Integer.parseInt(idText);
    }

    private static Path getBodyPath(Path headerPath, int bookId) {
        return headerPath.getParent()
                .resolve(bookId + ".body.txt");
    }
}
