package datamart.control;

import datamart.model.Metadata;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import static datamart.control.TextNormalizer.getWordsList;

public class HierarchicalInvertedIndexBuilder {

    private final Map<String, Set<Integer>> index =
            new HashMap<>();

    public void processBooks(List<Metadata> books) throws IOException {
        for (Metadata bookMetadata : books) {
            for (String word : getWordsList(bookMetadata)) {
                index
                        .computeIfAbsent(
                                word,
                                ignored -> new HashSet<>()
                        )
                        .add(bookMetadata.bookId());
            }
        }
    }

    public void save(Path outputDirectory) throws IOException {
        Files.createDirectories(outputDirectory);

        for (Map.Entry<String, Set<Integer>> entry : index.entrySet()) {
            String word = entry.getKey();

            Path letterDirectory = createLetterDirectory(outputDirectory, word);
            writeWordFile(entry, letterDirectory, word);
        }
    }

    private static Path createLetterDirectory(Path outputDirectory, String word) throws IOException {
        String firstLetter = word.substring(0, 1).toUpperCase();
        Path letterDirectory = outputDirectory.resolve(firstLetter);

        Files.createDirectories(letterDirectory);
        return letterDirectory;
    }

    private static void writeWordFile(Map.Entry<String, Set<Integer>> entry, Path letterDirectory, String word) throws IOException {
        Path wordFile = letterDirectory.resolve(word + ".txt");

        Files.writeString(
                wordFile,
                getWordContent(entry),
                StandardCharsets.UTF_8
        );
    }

    private static String getWordContent(Map.Entry<String, Set<Integer>> entry) {
        StringBuilder content = new StringBuilder();

        for (Integer bookId : entry.getValue()) {
            content
                    .append(bookId)
                    .append(System.lineSeparator());
        }
        return content.toString();
    }

    public int getNumberOfTerms() {
        return index.size();
    }
}
