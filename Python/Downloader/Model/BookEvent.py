from dataclasses import dataclass

@dataclass(frozen=True)
class BookEvent:
    date: str
    hour: str
    ss: str
    book_id: int
    type: str
    content: str
