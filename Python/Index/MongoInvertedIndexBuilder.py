from pymongo import MongoClient, ASCENDING, UpdateOne
from Index.InvertedIndexBuilder import InvertedIndexBuilder
from Index.MetaData import Metadata
from Index.TextNormalizer import TextNormalizer


class MongoInvertedIndexBuilder(InvertedIndexBuilder):
    def __init__(self, connection_string: str = "mongodb://localhost:27017"):
        self.client = MongoClient(connection_string, serverSelectionTimeoutMS=1000)
        self.db = self.client["datamart"]
        self.collection = self.db["inverted_index"]

    @staticmethod
    def is_server_available(host: str = "localhost", port: int = 27017, timeout: float = 0.5) -> bool:
        """Comprueba de forma instantánea mediante un socket TCP si el puerto de MongoDB está abierto."""
        import socket
        try:
            with socket.create_connection((host, port), timeout=timeout):
                return True
        except OSError:
            return False

    def ensure_index(self):
        """Crea el índice único sobre 'term'."""
        try:
            self.collection.create_index([("term", ASCENDING)], unique=True)
        except Exception:
            pass


    def process_book(self, book_metadata: Metadata) -> None:
        """Indexa un solo libro en MongoDB usando bulk_write."""
        words = TextNormalizer.get_words_list(book_metadata)
        if not words:
            return

        operations = [
            UpdateOne(
                {"term": word},
                {"$addToSet": {"postings": book_metadata.book_id}},
                upsert=True
            )
            for word in words
        ]
        self.collection.bulk_write(operations, ordered=False)

    def get_postings(self, term: str) -> list[int]:
        """Devuelve los IDs de los libros donde aparece el término."""
        normalized = term.lower().strip()
        doc = self.collection.find_one({"term": normalized})
        return doc["postings"] if doc and "postings" in doc else []

    def get_number_of_terms(self) -> int:
        """Devuelve la cantidad de términos indexados en MongoDB."""
        return self.collection.count_documents({})

    def close(self):
        self.client.close()
