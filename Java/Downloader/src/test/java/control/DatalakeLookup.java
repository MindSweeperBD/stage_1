package control;

import model.DatalakeLayout;

import java.io.File;

public class DatalakeLookup {

    public static File findBookPart(DatalakeLayout layout, String bookId, String part) {
        return switch (layout) {
            case TIME_BASED -> findTimeBased(bookId, part);
            case BOOK_BASED -> findBookBased(bookId, part);
            case BATCH_BASED -> findBatchBased(bookId, part);
        };
    }

    private static File findTimeBased(String bookId, String part) {
        File rootFolder = new File("time_datalake");

        for (File dateFolder : safeList(rootFolder)) {
            for (File hourFolder : safeList(dateFolder)) {
                File candidate = new File(hourFolder, bookId + "." + part + ".txt");
                if (candidate.exists()) return candidate;
            }
        }
        return null;
    }

    private static File findBookBased(String bookId, String part) {
        File bookFolder = new File("book_datalake" + File.separator + bookId);
        File candidate = new File(bookFolder, part + ".txt");
        return candidate.exists() ? candidate : null;
    }
    private static File findBatchBased(String bookId, String part) {
        String batchName = getBatchRange(bookId);
        File batchFolder = new File("batch_datalake" + File.separator + batchName);
        File candidate = new File(batchFolder, bookId + "." + part + ".txt");
        return candidate.exists() ? candidate : null;
    }

    private static File[] safeList(File folder) {
        File[] list = folder.listFiles();
        return list != null ? list : new File[0];
    }

    private static String getBatchRange(String id) {
        int bookId = Integer.parseInt(id);
        int batchSize = 1000;
        int start = (bookId / batchSize) * batchSize;
        int end = start + batchSize - 1;
        return start + "-" + end;
    }
}

