import time
from typing import Optional
from mindsweeper.model.book import Book
from mindsweeper.model.BookEvent import BookEvent
from mindsweeper.control.BookDownloader import BookDownloader


class BookFeeder:

    @staticmethod
    def get_book_event(book_id: int) -> Optional[BookEvent]:
        book = BookDownloader.download_book(book_id)
        if book is None:
            return None
        return BookFeeder._create_event(book_id, book)

    @staticmethod
    def _create_event(book_id: int, book: Book) -> BookEvent:
        #System.currentTimeMillis() en Java equivale a int(time.time() * 1000)
        current_time_ms = int(time.time() * 1000)
        return BookEvent(
            ts=current_time_ms,
            ss="BookFeeder",
            bookId=book_id,
            header=book.header,
            body=book.body
        )
