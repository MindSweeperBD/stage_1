package datamart;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.*;

public class InvertedIndexBuilder {

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

    private Set<String> getWordsList(Metadata bookMetadata) throws IOException {
        Path bodyPath = Path.of(bookMetadata.bodyPath());
        String content = Files.readString(bodyPath, StandardCharsets.UTF_8);
        return normalize(content);
    }

    private Set<String> normalize(String text) {
        Set<String> words = new HashSet<>();

        for (String word : getNormalizedWordList(text)) {
            if (word.length() > 1) {
                words.add(word);
            }
        }
        return words;
    }

    private String[] getNormalizedWordList(String text) {
        String IS_UNICODE_OR_NUMBER = "[^\\p{L}\\p{N}]+";
        String normalized = text
                .toLowerCase(Locale.ROOT)
                .replaceAll(IS_UNICODE_OR_NUMBER, " ");

        return normalized.split("\\s+");
    }

    public void saveInvertedIndex(Path outputPath) throws IOException {
        Map<String, List<Integer>> orderedIndex = new TreeMap<>();
        sortIndexes(orderedIndex);
        writeIndexesInFile(outputPath, orderedIndex);
    }

    private void sortIndexes(Map<String, List<Integer>> orderedIndex) {
        for (Map.Entry<String, Set<Integer>> entry : index.entrySet()) {
            List<Integer> booksIds = new ArrayList<>(entry.getValue());

            Collections.sort(booksIds);
            orderedIndex.put(entry.getKey(), booksIds);
        }
    }

    private static void writeIndexesInFile(Path outputPath, Map<String, List<Integer>> orderedIndex) throws IOException {
        ObjectMapper mapper = new ObjectMapper();

        mapper.enable(SerializationFeature.INDENT_OUTPUT);
        mapper.writeValue(outputPath.toFile(), orderedIndex);
    }

    public int getNumberOfTerms() {
        return index.size();
    }
}
