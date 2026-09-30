import logging
import sys
from pathlib import Path
from Downloader.Model.DatalakeLayout import DatalakeLayout
from Benchmark.BenchmarkRunner import BenchmarkRunner
from Benchmark.IndexBenchmarkRunner import IndexBenchmarkRunner
from Index.DatalakeReader import DatalakeReader

logging.basicConfig(level=logging.INFO,format="%(asctime)s [%(levelname)s] %(message)s",stream=sys.stdout)

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

    reader = DatalakeReader(Path("batch_datalake"))
    books = reader.read_metadata()
    if books:
        index_runner = IndexBenchmarkRunner()
        index_runner.benchmark_all(books)


if __name__ == "__main__":
    main()


