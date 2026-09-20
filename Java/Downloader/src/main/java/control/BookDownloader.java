package control;

import model.Book;

import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.charset.StandardCharsets;

public class BookDownloader {

    private static final String START_MARKER = "*** START OF THE PROJECT GUTENBERG EBOOK";
    private static final String END_MARKER = "*** END OF THE PROJECT GUTENBERG EBOOK";

    public static Book downloadBook(int bookId) throws IOException, InterruptedException {
        String text = getBookText(bookId);

        if (isInseparable(text)) return null;
        return getSplitBook(text);
    }

    private static String getBookText(int bookId) throws IOException, InterruptedException {
        String url = "https://www.gutenberg.org/cache/epub/" + bookId + "/pg" + bookId + ".txt";
        HttpResponse<String> response = getHttpResponse(url);

        return response.body();
    }

    private static HttpResponse<String> getHttpResponse(String url) throws IOException, InterruptedException {
        HttpClient client = HttpClient.newHttpClient();
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(url))
                .build();

        return client.send(
                request,
                HttpResponse.BodyHandlers.ofString(StandardCharsets.UTF_8)
        );
    }

    private static boolean isInseparable(String text) {
        return !text.contains(START_MARKER) || !text.contains(END_MARKER);
    }

    private static Book getSplitBook(String text) {
        int start = text.indexOf(START_MARKER);
        int end = text.indexOf(END_MARKER);

        String header = text.substring(0, start);
        String body = text.substring(start + START_MARKER.length(), end);

        return new Book(header, body);
    }
}

