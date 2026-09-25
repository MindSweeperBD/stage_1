package datamart.control;

import datamart.model.Metadata;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.HashSet;
import java.util.Locale;
import java.util.Set;

public class TextNormalizer {
    public static Set<String> getWordsList(Metadata bookMetadata) throws IOException {
        Path bodyPath = Path.of(bookMetadata.bodyPath());
        String content = Files.readString(bodyPath, StandardCharsets.UTF_8);
        return normalize(content);
    }

    private static Set<String> normalize(String text) {
        Set<String> words = new HashSet<>();

        for (String word : getNormalizedWordList(text)) {
            if (word.length() > 1) {
                words.add(word);
            }
        }
        return words;
    }

    private static String[] getNormalizedWordList(String text) {
        String IS_UNICODE_OR_NUMBER = "[^\\p{L}\\p{N}]+";
        String normalized = text
                .toLowerCase(Locale.ROOT)
                .replaceAll(IS_UNICODE_OR_NUMBER, " ");

        return normalized.split("\\s+");
    }
}
