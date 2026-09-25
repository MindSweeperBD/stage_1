package datamart.control;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;
import datamart.model.Metadata;

import java.io.IOException;
import java.nio.file.Path;
import java.util.*;

import static datamart.control.TextNormalizer.getWordsList;

public class MonolithicInvertedIndexBuilder {

    private final Map<String, Set<Integer>> index =
            new HashMap<>();

    public void processBooks(List<Metadata> books) throws IOException {
        for (Metadata bookMetadata : books) {
            processBook(bookMetadata);
        }
    }

    public void processBook(Metadata bookMetadata) throws IOException {
        for (String word : getWordsList(bookMetadata)) {
            index
                    .computeIfAbsent(
                            word,
                            ignored -> new HashSet<>()
                    )
                    .add(bookMetadata.bookId());
        }
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
