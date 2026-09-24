from pathlib import Path
from Downloader.Control.EventStore import EventStore
from Downloader.Model.BookEvent import BookEvent
from Downloader.Model.DatalakeLayout import DatalakeLayout

class EventStoreBuilder(EventStore):
    def __init__(self, layout: DatalakeLayout):
        self.layout = layout
        self.base_dir = "datalake"

    def store(self, book: BookEvent) -> None:
        target_file = self._resolve_event_file(book.book_id, book.type, book.date, book.hour)
        target_file.parent.mkdir(parents=True, exist_ok=True)
        with open(target_file, "a", encoding="utf-8") as f:
            f.write(book.content + "\n")

    def _resolve_event_file(self, book_id: int, type_: str, date: str, hour: str) -> Path:
        if self.layout == DatalakeLayout.TIME_BASED:
            return Path(f"time_{self.base_dir}") / date / hour / f"{book_id}.{type_}.txt"
        elif self.layout == DatalakeLayout.BOOK_BASED:
            return Path(f"book_{self.base_dir}") / str(book_id) / f"{type_}.txt"
        elif self.layout == DatalakeLayout.BATCH_BASED:
            batch_folder = self._get_batch_range(book_id)
            return Path(f"batch_{self.base_dir}") / batch_folder / f"{book_id}.{type_}.txt"

    @staticmethod
    def _get_batch_range(book_id: int) -> str:
        batch_size = 1000
        start = (book_id // batch_size) * batch_size
        end = start + batch_size - 1
        return f"{start}-{end}"
