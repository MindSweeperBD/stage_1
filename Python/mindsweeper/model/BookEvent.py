from dataclasses import dataclass, asdict

@dataclass(frozen=True)
class BookEvent:
    ts: int       # Timestamp en milisegundos (long en Java)
    ss: str       # Subsystem / Source ("BookFeeder")
    bookId: int   # ID del libro en Gutenberg
    header: str   # Cabecera con metadatos
    body: str     # Contenido del libro

    def to_dict(self) -> dict:
        """Convierte el objeto a diccionario para serializarlo a JSON idéntico a Gson."""
        return asdict(self)
