import logging
import time
from pathlib import Path
from Downloader.Control.BookFeeder import BookFeeder
from Downloader.Model.DatalakeLayout import DatalakeLayout
from Benchmark.DatalakeLookup import DatalakeLookup
from Benchmark.DatalakeProcessing import DatalakeProcessing
from Benchmark.DatalakeRecovery import DatalakeRecovery
from Benchmark.DatalakeStorage import DatalakeStorage

log = logging.getLogger(__name__)

class BenchmarkRunner:
    def __init__(self, layout: DatalakeLayout):
        self.layout = layout
        self.indexed_file = Path("control/indexed_books.txt")

    def benchmark_throughput(self, book_ids: list[int]):
        start = time.perf_counter()
        BookFeeder.save_books(book_ids, self.layout)
        seconds = time.perf_counter() - start
        rate = len(book_ids) / seconds if seconds > 0 else 0
        log.info(f"{self.layout.value} layout: {len(book_ids)} books in {seconds:.2f}s ({rate:.4f} books/s)")

    def benchmark_lookup(self, book_id: str):
        start = time.perf_counter()
        header = DatalakeLookup.find_book_part(self.layout, book_id, "header")
        body = DatalakeLookup.find_book_part(self.layout, book_id, "body")
        millis = (time.perf_counter() - start) * 1000
        log.info(f"{self.layout.value} layout: book {book_id} found in {millis:.4f}ms (header={header is not None}, body={body is not None})")

    def benchmark_incremental_processing(self):
        start = time.perf_counter()
        indexed = set()
        if self.indexed_file.exists():
            indexed = set(self.indexed_file.read_text(encoding="utf-8").splitlines())
        present = DatalakeProcessing.scan_book_ids(self.layout)
        new_books = present - indexed
        millis = (time.perf_counter() - start) * 1000
        log.info(f"{self.layout.value} layout: {len(new_books)} new books detected in {millis:.4f}ms")

    def benchmark_recovery(self, book_ids: list[int]):
        partial = book_ids[:len(book_ids) // 2]
        BookFeeder.save_books(partial, self.layout)
        timestamps = BookFeeder.save_books(book_ids, self.layout)

        start = time.perf_counter()
        rec = DatalakeRecovery(self.layout, timestamps, book_ids)
        if rec.verify_recovery():
            millis = (time.perf_counter() - start) * 1000
            log.info(f"{self.layout.value} layout: recovery OK in {millis:.4f}ms (no duplicates, no missing docs)")
        else:
            log.error(f"{self.layout.value} layout: recovery FAILED")

    def benchmark_storage(self):
        stats = DatalakeStorage(self.layout).measure()
        log.info(f"{self.layout.value} layout: {stats.directories} dirs, {stats.files} files, {stats.auxiliary} auxiliary")

