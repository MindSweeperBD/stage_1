package model;

public class BookEvent {

    private final String date;
    private final String hour;
    private final String ss;
    private final int bookId;
    private final String type;
    private final String content;

    public BookEvent(String date, String hour, String ss, int bookId, String header, String body) {
        this.date = date;
        this.hour = hour;
        this.ss = ss;
        this.bookId = bookId;
        this.type = header;
        this.content = body;
    }

    public String getDate() { return date; }
    public String getHour() { return hour; }
    public String getSs() { return ss; }
    public int getBookId() { return bookId; }
    public String getType() { return type; }
    public String getContent() { return content; }
}

