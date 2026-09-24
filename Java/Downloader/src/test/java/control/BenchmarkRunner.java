package control;

import model.DatalakeLayout;
import model.StorageStats;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.*;

import static control.BookFeeder.saveBooks;

public class BenchmarkRunner {

    private final DatalakeLayout layout;

    private static final Logger log = LoggerFactory.getLogger(BenchmarkRunner.class);
    private static final File indexedFile = new File("control/indexed_books.txt");

    public BenchmarkRunner(DatalakeLayout layout){
        this.layout = layout;
    }

    public void benchmarkThroughput(List<Integer> bookIds) throws IOException, InterruptedException {
        long start = System.nanoTime();
        saveBooks(bookIds, layout);
        double seconds = (System.nanoTime() - start) / 1000000000.0;
        throughputLog(bookIds, seconds);
    }

    private void throughputLog(List<Integer> bookIds, double seconds) {
        log.info("{} layout: {} books in {} s ({} books/s)",
                layout,
                bookIds.size(),
                String.format("%.2f", seconds),
                String.format("%.4f", bookIds.size() / seconds));
    }


    public void benchmarkLookup(String bookId) {
        long start = System.nanoTime();

        File header = DatalakeLookup.findBookPart(layout, bookId, "body");
        File body = DatalakeLookup.findBookPart(layout, bookId, "header");

        double millis = (System.nanoTime() - start) / 1000000.0;
        lookupLog(bookId, millis, header, body);
    }

    private void lookupLog(String bookId, double millis, File header, File body) {
        log.info("{} layout: book {} found in {} ms (header={}, body={})",
                layout,
                bookId,
                String.format("%.2f", millis),
                header != null, body != null);
    }


    public void benchmarkIncrementalProcessing() throws IOException {
        long start = System.nanoTime();

        Set<String> indexed = indexedFile.exists()
                ? new HashSet<>(Files.readAllLines(indexedFile.toPath()))
                : new HashSet<>();

        Set<String> present = DatalakeProcessing.scanBookIds(layout);
        present.removeAll(indexed);

        double millis = (System.nanoTime() - start) / 1000000.0;
        incrementalProcessingLog(present, millis);
    }

    private void incrementalProcessingLog(Set<String> present, double millis) {
        log.info("{} layout: {} new books detected in {} ms",
                layout,
                present.size(),
                String.format("%.2f", millis));
    }


    public void benchmarkRecovery(List<Integer> bookIds) throws Exception {
        Map<Integer, String[]> timestamps = interruptionExample(bookIds);
        long start = System.nanoTime();

        DatalakeRecovery recovery = new DatalakeRecovery(
                layout,
                timestamps,
                bookIds
        );
        if (recovery.verifyRecovery()) {
            long end = System.nanoTime();
            double millis = (end - start) / 1000000.0;
            recoveryLog(millis);
        } else {
            log.error("{} layout: recovery FAILED", layout);
        }
    }

    private Map<Integer, String[]> interruptionExample(List<Integer> bookIds) throws IOException, InterruptedException {
        List<Integer> partial = bookIds.subList(0, bookIds.size() / 2);
        saveBooks(partial, layout);
        return saveBooks(bookIds, layout);
    }

    private void recoveryLog(double millis) {
        log.info("{} layout: recovery OK in {} ms (no duplicates, no missing documents)",
                layout, String.format("%.4f", millis));
    }


    public void benchmarkStorage() throws IOException {
        DatalakeStorage storage = new DatalakeStorage(layout);
        StorageStats stats = storage.measure();
        storageLog(stats);
    }

    private void storageLog(StorageStats stats) {
        log.info("{} layout: {} dirs, {} files, {} auxiliary",
                layout,
                stats.directories(),
                stats.files(),
                stats.auxiliary());
    }
}
