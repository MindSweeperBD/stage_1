from abc import ABC, abstractmethod
from Index.MetaData import Metadata


class InvertedIndexBuilder(ABC):
    @abstractmethod
    def process_book(self, book_metadata: Metadata) -> None:
        """indexa un único libro."""
        pass

    def process_books(self, books: list[Metadata]) -> None:
        """procesa una lista de libros."""
        for book in books:
            self.process_book(book)

    @abstractmethod
    def get_postings(self, term: str) -> list[int]:
        """Devuelve los IDs de los libros donde aparece el término."""
        pass
