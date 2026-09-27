from dataclasses import dataclass

@dataclass(frozen=True)
class Metadata:
    book_id: int
    title: str
    author: str
    language: str
    body_path: str
