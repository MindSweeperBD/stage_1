package datamart;

public record Metadata(int bookId, String title, String author, String language, String bodyPath) {

    @Override
    public String toString() {
        return "Metadata{" +
                "bookId=" + bookId +
                ", title='" + title + '\'' +
                ", author='" + author + '\'' +
                ", language='" + language + '\'' +
                ", bodyPath='" + bodyPath + '\'' +
                '}';
    }
}
