from typing import Optional
import requests
from Downloader.Model.Book import Book

START_MARKER = "*** START OF THE PROJECT GUTENBERG EBOOK"
END_MARKER = "*** END OF THE PROJECT GUTENBERG EBOOK"

class BookDownloader:
    @staticmethod
    def download_book(book_id: int) -> Optional[Book]:
        try:
            url = f"https://www.gutenberg.org/cache/epub/{book_id}/pg{book_id}.txt"
            resp = requests.get(url, headers={"User-Agent": "MindSweeper/1.0"})
            if resp.status_code != 200:
                return None
            text = resp.text
            if START_MARKER not in text or END_MARKER not in text:
                return None

            start = text.find(START_MARKER)
            end = text.find(END_MARKER)
            header = text[:start]
            body = text[start + len(START_MARKER):end]
            return Book(header=header, body=body)
        except Exception:
            return None
