import model.DatalakeLayout;
import control.BenchmarkRunner;

import java.util.List;

import static model.DatalakeLayout.TIME_BASED;
import static model.DatalakeLayout.BOOK_BASED;
import static model.DatalakeLayout.BATCH_BASED;


public class Main {
    private static final List<DatalakeLayout> layouts = List.of(TIME_BASED, BOOK_BASED, BATCH_BASED);

    public static void main(String[] args) throws Exception {
        for (DatalakeLayout layout : layouts) {
            BenchmarkRunner runner = new BenchmarkRunner(layout);
            runner.benchmarkThroughput(List.of(1342, 1610, 99, 2700));
            runner.benchmarkLookup("1342");
            runner.benchmarkIncrementalProcessing();
            runner.benchmarkRecovery(List.of(1342, 1610, 99, 2700));
            runner.benchmarkStorage();
        }
    }
}
