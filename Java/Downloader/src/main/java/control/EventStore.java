package control;

import model.BookEvent;

public interface EventStore {
    void store(BookEvent book);
}
