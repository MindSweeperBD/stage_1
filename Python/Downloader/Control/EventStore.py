from abc import ABC, abstractmethod
from Downloader.Model.BookEvent import BookEvent

class EventStore(ABC):
    @abstractmethod
    def store(self, book: BookEvent) -> None:
        pass
