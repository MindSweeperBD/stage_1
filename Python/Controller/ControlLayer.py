import logging
import sys
import random
from pathlib import Path

from Downloader.Control.BookFeeder import BookFeeder
from Downloader.Control.EventStoreBuilder import EventStoreBuilder
from Downloader.Model.DatalakeLayout import DatalakeLayout
from Index.DatalakeReader import DatalakeReader
from Index.MetaDataDataBase import MetadataDatabase
from Index.MonolithicInvertedIndexBuilder import MonolithicInvertedIndexBuilder
from Index.Indexer import Indexer

logging.basicConfig(level=logging.INFO,format="%(asctime)s [%(levelname)s] %(message)s", stream=sys.stdout)
log = logging.getLogger(__name__)


class ControlLayer:
    def __init__(self):
        self.control_path = Path("control")
        self.downloaded_books_file = self.control_path / "downloaded_books.txt"
        self.indexed_books_file = self.control_path / "indexed_books.txt"

        self.total_books = 70000
        self.used_layout = DatalakeLayout.BATCH_BASED

        self.datamart_path = Path("datamart")
        self.datalake_path = Path("batch_datalake")
        self.metadata_database = MetadataDatabase(str(self.datamart_path / "metadata.db"))
        self.monolithic_index_builder = MonolithicInvertedIndexBuilder()
        self.index_path = self.datamart_path / "inverted_index.json"

        self.control_path.mkdir(parents=True, exist_ok=True)
        self.datamart_path.mkdir(parents=True, exist_ok=True)

        self.monolithic_index_builder.load_inverted_index(self.index_path)

    def control_pipeline_step(self):
        downloaded = self._read_book_ids(self.downloaded_books_file)
        indexed = self._read_book_ids(self.indexed_books_file)

        ready_to_index = downloaded - indexed

        if not ready_to_index:
            self._download_new_book(downloaded)
        else:
            self._index_next_book(ready_to_index)

    def _read_book_ids(self, file_path: Path) -> set[str]:
        if not file_path.exists():
            return set()
        return set(file_path.read_text(encoding="utf-8").splitlines())

    def _download_new_book(self, downloaded: set[str]):
        for _ in range(100):
            candidate_id = random.randint(1, self.total_books)
            if str(candidate_id) not in downloaded:
                log.info(f"[CONTROL] Downloading book {candidate_id}...")
                self._download_book(candidate_id)
                log.info(f"[CONTROL] Book {candidate_id} successfully downloaded.")
                return
        log.info("[CONTROL] Could not find a new book.")

    def _download_book(self, book_id: int):
        builder = EventStoreBuilder(self.used_layout)
        events = BookFeeder._get_book_events(book_id)
        if not events:
            log.warning(f"No events found for book {book_id}")
            return
        for ev in events:
            builder.store(ev)
        BookFeeder.mark_as_downloaded(book_id)

    def _index_next_book(self, ready_to_index: set[str]):
        book_id = next(iter(ready_to_index))
        log.info(f"[CONTROL] Indexing book {book_id}...")
        self._index_book(book_id)
        log.info(f"[CONTROL] Book {book_id} successfully indexed.")

    def _index_book(self, book_id: str):
        self.metadata_database.create_database()
        book_metadata = self._get_metadata(book_id)

        self.metadata_database.insert_metadata([book_metadata])
        self.monolithic_index_builder.process_book(book_metadata)
        self.monolithic_index_builder.save_inverted_index(self.index_path)

        Indexer.mark_as_indexed(int(book_id))

    def _get_metadata(self, book_id: str):
        reader = DatalakeReader(self.datalake_path)
        metadata_list = reader.read_metadata()
        bid = int(book_id)
        for m in metadata_list:
            if m.book_id == bid:
                return m
        raise ValueError(f"Metadata not found for book: {book_id}")
