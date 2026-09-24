import model.DatalakeLayout;

import java.util.List;

import static control.BookFeeder.saveBooks;
import static model.DatalakeLayout.*;

public class DownloaderMain {
    private static final List<DatalakeLayout> layouts = List.of(TIME_BASED, BOOK_BASED, BATCH_BASED);
    private static final List<Integer> bookIds = List.of(99, 177, 1342, 1610, 2700);

    public static void main(String[] args) throws Exception {
        for (DatalakeLayout layout : layouts) {
            saveBooks(bookIds, layout);
        }
    }
}
