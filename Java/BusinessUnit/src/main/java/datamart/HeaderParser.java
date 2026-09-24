package datamart;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class HeaderParser {

    private static final Pattern TITLE_PATTERN =
            Pattern.compile("^Title:\\s(.+)$", Pattern.MULTILINE);

    private static final Pattern AUTHOR_PATTERN =
            Pattern.compile("^Author:\\s(.+)$", Pattern.MULTILINE);

    private static final Pattern LANGUAGE_PATTERN =
            Pattern.compile("^Language:\\s(.+)$", Pattern.MULTILINE);

    public static Metadata parse(Path headerPath, Path bodyPath, int bookId) throws IOException {
        String content = Files.readString(headerPath, StandardCharsets.UTF_8);
        String title = extract(TITLE_PATTERN, content);
        String author = extract(AUTHOR_PATTERN, content);
        String language = extract(LANGUAGE_PATTERN, content);

        return new Metadata(
                bookId,
                title,
                author,
                language,
                bodyPath.toString()
        );
    }

    private static String extract(Pattern pattern, String content) {
        Matcher matcher = pattern.matcher(content);

        if (matcher.find()) {
            return matcher.group(1).trim();
        }
        return "";
    }
}
