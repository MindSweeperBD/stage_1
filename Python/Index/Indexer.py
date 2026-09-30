from pathlib import Path
from Index.MetaData import Metadata

CONTROL_PATH = Path("control")
INDEXED_BOOKS = CONTROL_PATH / "indexed_books.txt"


class Indexer:
    @staticmethod
    def mark_list_as_indexed(books: list[Metadata]):
        for b in books:
            Indexer.mark_as_indexed(b.book_id)

    @staticmethod
    def mark_as_indexed(book_id: int):
        CONTROL_PATH.mkdir(parents=True, exist_ok=True)
        indexed = set()
        if INDEXED_BOOKS.exists():
            indexed = set(INDEXED_BOOKS.read_text(encoding="utf-8").splitlines())

        bid_str = str(book_id)
        if bid_str not in indexed:
            with open(INDEXED_BOOKS, "a", encoding="utf-8") as f:
                f.write(bid_str + "\n")