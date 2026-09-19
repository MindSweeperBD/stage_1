from abc import ABC, abstractmethod

class EventStore(ABC):

    @abstractmethod
    def store(self) -> None:
        pass
