from datetime import datetime
from pathlib import Path
from Downloader.Control.BookDownloader import BookDownloader
from Downloader.Control.EventStoreBuilder import EventStoreBuilder
#from Downloader.Model.Book import Book
from Downloader.Model.BookEvent import BookEvent
from Downloader.Model.DatalakeLayout import DatalakeLayout

INDEXED_FILE = Path("control/indexed_books.txt")

class BookFeeder:
    @staticmethod
    def save_books(book_ids: list[int], layout: DatalakeLayout) -> dict[int, list[str]]:
        builder = EventStoreBuilder(layout)
        timestamps = {}

        for book_id in book_ids:
            events = BookFeeder._get_book_events(book_id)
            if events:
                for ev in events:
                    builder.store(ev)
                    BookFeeder._add_timestamp(book_id, timestamps)
                BookFeeder.mark_as_downloaded(book_id)
        return timestamps

    @staticmethod
    def _add_timestamp(book_id: int, timestamps: dict):
        now = datetime.now()
        timestamps[book_id] = [now.strftime("%Y%m%d"), now.strftime("%H")]

    @staticmethod
    def _mark_as_indexed(book_id: int):
        INDEXED_FILE.parent.mkdir(parents=True, exist_ok=True)
        with open(INDEXED_FILE, "a+", encoding="utf-8") as f:
            f.seek(0)
            lines = [line.strip() for line in f.readlines()]
            if str(book_id) not in lines:
                f.write(f"{book_id}\n")

    @staticmethod
    def _get_book_events(book_id: int) -> list[BookEvent] | None:
        book = BookDownloader.download_book(book_id)
        if not book:
            return None
        now = datetime.now()
        date_str = now.strftime("%Y%m%d")
        hour_str = now.strftime("%H")

        header_ev = BookEvent(date_str, hour_str, "BookFeeder", book_id, "header", book.header)
        body_ev = BookEvent(date_str, hour_str, "BookFeeder", book_id, "body", book.body)
        return [header_ev, body_ev]

    @staticmethod
    def mark_as_downloaded(book_id: int):
        file_path = Path("control/downloaded_books.txt")
        file_path.parent.mkdir(parents=True, exist_ok=True)
        downloaded = set()
        if file_path.exists():
            downloaded = set(file_path.read_text(encoding="utf-8").splitlines())
        if str(book_id) not in downloaded:
            with open(file_path, "a", encoding="utf-8") as f:
                f.write(f"{book_id}\n")
