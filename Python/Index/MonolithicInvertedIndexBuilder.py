import json
from pathlib import Path
from Index.InvertedIndexBuilder import InvertedIndexBuilder
from Index.MetaData import Metadata
from Index.TextNormalizer import TextNormalizer


class MonolithicInvertedIndexBuilder(InvertedIndexBuilder):
    def __init__(self):
        self.index: dict[str, set[int]] = {}

    def process_book(self, book_metadata: Metadata) -> None:
        words = TextNormalizer.get_words_list(book_metadata)
        for word in words:
            if word not in self.index:
                self.index[word] = set()
            self.index[word].add(book_metadata.book_id)

    def save_inverted_index(self, output_path: Path) -> None:
        ordered_index = {
            word: sorted(list(self.index[word]))
            for word in sorted(self.index.keys())
        }
        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, "w", encoding="utf-8") as f:
            json.dump(ordered_index, f, indent=2, ensure_ascii=False)

    def load_inverted_index(self, input_path: Path) -> None:
        if not input_path.exists():
            return
        with open(input_path, "r", encoding="utf-8") as f:
            data = json.load(f)
            self.index = {word: set(postings) for word, postings in data.items()}

    def get_number_of_terms(self) -> int:
        return len(self.index)

    def get_postings(self, term: str) -> list[int]:
        normalized = term.lower().strip()
        return sorted(list(self.index.get(normalized, set())))
