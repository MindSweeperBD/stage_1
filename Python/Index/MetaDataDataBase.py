import sqlite3
from Index.MetaData import Metadata

class MetadataDatabase:
    def __init__(self, database_path: str):
        self.database_path = database_path

    def create_database(self) -> None:
        conn = sqlite3.connect(self.database_path)
        try:
            with conn:
                cursor = conn.cursor()
                cursor.execute("""
                    CREATE TABLE IF NOT EXISTS books (
                        book_id INTEGER PRIMARY KEY,
                        title TEXT NOT NULL,
                        author TEXT NOT NULL,
                        language TEXT NOT NULL,
                        body_path TEXT NOT NULL
                    )
                """)
                cursor.execute("CREATE INDEX IF NOT EXISTS idx_books_author ON books(author)")
                cursor.execute("CREATE INDEX IF NOT EXISTS idx_books_title ON books(title)")
                cursor.execute("CREATE INDEX IF NOT EXISTS idx_books_language ON books(language)")
        finally:
            conn.close()

    def insert_metadata(self, metadata_list: list[Metadata]) -> None:
        sql = """
            INSERT OR REPLACE INTO books
            (book_id, title, author, language, body_path)
            VALUES (?, ?, ?, ?, ?)
        """
        data = [
            (b.book_id, b.title, b.author, b.language, b.body_path)
            for b in metadata_list
        ]
        conn = sqlite3.connect(self.database_path)
        try:
            with conn:
                cursor = conn.cursor()
                cursor.executemany(sql, data)
        finally:
            conn.close()

    def find_by_author(self, author: str) -> list[Metadata]:
        sql = "SELECT book_id, title, author, language, body_path FROM books WHERE author = ?"
        conn = sqlite3.connect(self.database_path)
        try:
            cursor = conn.cursor()
            cursor.execute(sql, (author,))
            rows = cursor.fetchall()
            return [Metadata(*row) for row in rows]
        finally:
            conn.close()

    def find_body_path_by_id(self, book_id: int) -> str | None:
        sql = "SELECT body_path FROM books WHERE book_id = ?"
        conn = sqlite3.connect(self.database_path)
        try:
            cursor = conn.cursor()
            cursor.execute(sql, (book_id,))
            row = cursor.fetchone()
            return row[0] if row else None
        finally:
            conn.close()
