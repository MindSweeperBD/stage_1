from dataclasses import dataclass

#record en python para transportar datos
@dataclass(frozen=True)
class Book:
    header: str
    body: str