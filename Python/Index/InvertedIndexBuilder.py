from abc import ABC, abstractmethod
from Index.MetaData import Metadata


class InvertedIndexBuilder(ABC):
    @abstractmethod
    def process_book(self, book_metadata: Metadata) -> None:
        pass

    def process_books(self, books: list[Metadata]) -> None:
        for book in books:
            self.process_book(book)

    @abstractmethod
    def get_postings(self, term: str) -> list[int]:
        pass
