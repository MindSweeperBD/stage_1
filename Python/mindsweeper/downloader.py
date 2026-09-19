from pathlib import Path
import requests
from Python.mindsweeper.model.book import Book

START_MARKER = "*** START OF THE PROJECT GUTENBERG EBOOK"
END_MARKER = "*** END OF THE PROJECT GUTENBERG EBOOK"

def download_book(book_id: int) -> bool:
    """Descarga, divide y guarda un libro por su ID."""
    try:
        text = _get_book_text(book_id)
    except requests.RequestException as e:
        print(f"[ERROR] Error de red descargando el libro {book_id}: {e}")
        return False
    if _is_inseparable(text):
        return False
    book = _get_split_book(text)
    _save_book(book_id, book)
    return True

def _save_book(book_id: int, book: Book) -> None:
    output_path = _get_books_path()
    body_path = output_path / f"{book_id}_body.txt"
    header_path = output_path / f"{book_id}_header.txt"
    # write_text con utf-8 equivale a Files.writeString en Java
    body_path.write_text(book.body.strip(), encoding="utf-8")
    header_path.write_text(book.header.strip(), encoding="utf-8")

def _get_split_book(text: str) -> Book:
    # indexOf en Java equivale a .find() en Python
    start = text.find(START_MARKER)
    end = text.find(END_MARKER)
    # Substrings mediante slicing de Python [inicio:fin]
    header = text[:start]
    body = text[start + len(START_MARKER):end]
    return Book(header=header, body=body)

def _is_inseparable(text: str) -> bool:
    return (START_MARKER not in text) or (END_MARKER not in text)

def _get_book_text(book_id: int) -> str:
    url = f"https://www.gutenberg.org/cache/epub/{book_id}/pg{book_id}.txt"
    response = _get_http_response(url)
    return response.text

def _get_http_response(url: str) -> requests.Response:
    headers = {"User-Agent": "MindSweeper-SearchEngine/1.0"}
    response = requests.get(url, headers=headers)
    response.raise_for_status()
    return response

def _get_books_path() -> Path:
    output_path = Path("data/output")
    output_path.mkdir(parents=True, exist_ok=True)
    return output_path