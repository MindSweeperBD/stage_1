import logging
from Downloader.Model.DatalakeLayout import DatalakeLayout
from Benchmark.BenchmarkRunner import BenchmarkRunner

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")

def main():
    layouts = [
        DatalakeLayout.TIME_BASED,
        DatalakeLayout.BOOK_BASED,
        DatalakeLayout.BATCH_BASED
    ]
    books_to_test = [1342, 1610, 99, 2700]

    for layout in layouts:
        print(f"\n==================== RUNNING FOR {layout.value} ====================")
        runner = BenchmarkRunner(layout)
        runner.benchmark_throughput(books_to_test)
        runner.benchmark_lookup("1342")
        runner.benchmark_incremental_processing()
        runner.benchmark_recovery(books_to_test)
        runner.benchmark_storage()

if __name__ == "__main__":
    main()

