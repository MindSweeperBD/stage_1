from typing import Optional
import requests
from mindsweeper.model.book import Book

START_MARKER = "*** START OF THE PROJECT GUTENBERG EBOOK"
END_MARKER = "*** END OF THE PROJECT GUTENBERG EBOOK"

class BookDownloader:

    @staticmethod
    def download_book(book_id: int) -> Optional[Book]:
        try:
            text = BookDownloader._get_book_text(book_id)
        except requests.RequestException as e:
            print(f"[ERROR] Error de red descargando el libro {book_id}: {e}")
            return None

        if BookDownloader._is_inseparable(text):
            return None

        return BookDownloader._get_split_book(text)

    @staticmethod
    def _get_split_book(text: str) -> Book:
        start = text.find(START_MARKER)
        end = text.find(END_MARKER)

        header = text[:start]
        body = text[start + len(START_MARKER):end]

        return Book(header=header, body=body)

    @staticmethod
    def _is_inseparable(text: str) -> bool:
        return (START_MARKER not in text) or (END_MARKER not in text)

    @staticmethod
    def _get_book_text(book_id: int) -> str:
        url = f"https://www.gutenberg.org/cache/epub/{book_id}/pg{book_id}.txt"
        response = requests.get(url, headers={"User-Agent": "MindSweeper-SearchEngine/1.0"})
        response.raise_for_status()
        return response.text