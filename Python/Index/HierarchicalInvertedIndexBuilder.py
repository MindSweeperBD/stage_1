from pathlib import Path
from Index.InvertedIndexBuilder import InvertedIndexBuilder
from Index.MetaData import Metadata
from Index.TextNormalizer import TextNormalizer


class HierarchicalInvertedIndexBuilder(InvertedIndexBuilder):
    def __init__(self, base_directory: Path = Path("datamart/hierarchical_index")):
        self.base_directory = Path(base_directory)
        self.index: dict[str, set[int]] = {}

    def process_book(self, book_metadata: Metadata) -> None:
        """Procesa un solo libro e inserta sus palabras en el índice."""
        for word in TextNormalizer.get_words_list(book_metadata):
            if word not in self.index:
                self.index[word] = set()
            self.index[word].add(book_metadata.book_id)

    def save(self, output_directory: Path = None):
        target_dir = Path(output_directory) if output_directory else self.base_directory
        self.base_directory = target_dir
        target_dir.mkdir(parents=True, exist_ok=True)
        created_dirs = set()
        for word, book_ids in self.index.items():
            first_letter = word[0].upper()
            letter_dir = target_dir / first_letter
            if first_letter not in created_dirs:
                letter_dir.mkdir(parents=True, exist_ok=True)
                created_dirs.add(first_letter)

            word_file = letter_dir / f"{word}.txt"
            content = "\n".join(str(bid) for bid in sorted(book_ids)) + "\n"
            try:
                word_file.write_text(content, encoding="utf-8")
            except OSError:
                pass

    def get_number_of_terms(self) -> int:
        return len(self.index)

    def get_postings(self, term: str) -> list[int]:
        """Devuelve los IDs de los libros donde aparece el término."""
        normalized = term.lower().strip()
        if not normalized:
            return []
        first_letter = normalized[0].upper()
        word_file = self.base_directory / first_letter / f"{normalized}.txt"
        if word_file.exists():
            return [int(line.strip()) for line in word_file.read_text(encoding="utf-8").splitlines() if line.strip()]
        return sorted(list(self.index.get(normalized, set())))
