from dataclasses import dataclass

@dataclass(frozen=True)
class Book:
    header: str
    body: str
