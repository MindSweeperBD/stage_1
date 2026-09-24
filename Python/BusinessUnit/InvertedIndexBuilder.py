import json
import re
from pathlib import Path
from BusinessUnit.MetaData import Metadata

IS_NOT_UNICODE_OR_NUMBER = re.compile(r"[^\w]|_", re.UNICODE)

class InvertedIndexBuilder:
    def __init__(self):
        self.index: dict[str, set[int]] = {}

    def process_books(self, books: list[Metadata]) -> None:
        for book_metadata in books:
            words = self._get_words_list(book_metadata)
            for word in words:
                if word not in self.index:
                    self.index[word] = set()
                self.index[word].add(book_metadata.book_id)

    def _get_words_list(self, book_metadata: Metadata) -> set[str]:
        body_path = Path(book_metadata.body_path)
        if not body_path.exists():
            return set()
        content = body_path.read_text(encoding="utf-8", errors="ignore")
        return self._normalize(content)

    def _normalize(self, text: str) -> set[str]:
        normalized = IS_NOT_UNICODE_OR_NUMBER.sub(" ", text.lower())
        words = normalized.split()
        return {w for w in words if len(w) > 1}

    def save_inverted_index(self, output_path: Path) -> None:
        # Ordenamos las palabras alfabéticamente y los IDs numéricamente
        ordered_index = {
            word: sorted(list(self.index[word]))
            for word in sorted(self.index.keys())
        }
        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, "w", encoding="utf-8") as f:
            json.dump(ordered_index, f, indent=2, ensure_ascii=False)

    def get_number_of_terms(self) -> int:
        return len(self.index)
