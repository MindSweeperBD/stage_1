package benchmark;

import control.BenchmarkRunner;
import org.openjdk.jmh.annotations.*;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.TimeUnit;

import static model.DatalakeLayout.*;

@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.MILLISECONDS)
@State(Scope.Benchmark)
public class BenchmarkJHM {

    private static final List<Integer> BOOK_IDS =
            List.of(1342, 1610, 99, 2700);
    public static final String BOOK_TO_FIND = "1342";

    private BenchmarkRunner timeBasedRunner;
    private BenchmarkRunner bookBasedRunner;
    private BenchmarkRunner batchBasedRunner;

    @Setup(Level.Trial)
    public void setup() {
        timeBasedRunner = new BenchmarkRunner(TIME_BASED);
        bookBasedRunner = new BenchmarkRunner(BOOK_BASED);
        batchBasedRunner = new BenchmarkRunner(BATCH_BASED);
    }

    @Benchmark
    public void throughputTimeBased() throws Exception {
        timeBasedRunner.benchmarkThroughput(BOOK_IDS);
    }

    @Benchmark
    public void throughputBookBased() throws Exception {
        bookBasedRunner.benchmarkThroughput(BOOK_IDS);
    }

    @Benchmark
    public void throughputBatchBased() throws Exception {
        batchBasedRunner.benchmarkThroughput(BOOK_IDS);
    }

    @Benchmark
    public void lookUpTimeBased() {
        timeBasedRunner.benchmarkLookup(BOOK_TO_FIND);
    }

    @Benchmark
    public void lookUpBookBased() {
        bookBasedRunner.benchmarkLookup(BOOK_TO_FIND);
    }

    @Benchmark
    public void lookUpBatchBased() {
        batchBasedRunner.benchmarkLookup(BOOK_TO_FIND);
    }

    @Benchmark
    public void incrementalProcessingTimeBased() throws IOException {
        timeBasedRunner.benchmarkIncrementalProcessing();
    }

    @Benchmark
    public void incrementalProcessingBookBased() throws IOException {
        bookBasedRunner.benchmarkIncrementalProcessing();
    }

    @Benchmark
    public void incrementalProcessingBatchBased() throws IOException {
        batchBasedRunner.benchmarkIncrementalProcessing();
    }

    @Benchmark
    public void recoveryTimeBased() throws Exception {
        timeBasedRunner.benchmarkRecovery(BOOK_IDS);
    }

    @Benchmark
    public void recoveryBookBased() throws Exception {
        bookBasedRunner.benchmarkRecovery(BOOK_IDS);
    }

    @Benchmark
    public void recoveryBatchBased() throws Exception {
        batchBasedRunner.benchmarkRecovery(BOOK_IDS);
    }

    @Benchmark
    public void storageTimeBased() throws Exception {
        timeBasedRunner.benchmarkStorage();
    }

    @Benchmark
    public void storageBookBased() throws Exception {
        bookBasedRunner.benchmarkStorage();
    }

    @Benchmark
    public void storageBatchBased() throws Exception {
        batchBasedRunner.benchmarkStorage();
    }
}