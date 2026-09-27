import re
from pathlib import Path
from Index.MetaData import Metadata

IS_NOT_UNICODE_OR_NUMBER = re.compile(r"[^\w]|_", re.UNICODE)

class TextNormalizer:
    @staticmethod
    def get_words_list(book_metadata: Metadata) -> set[str]:
        body_path = Path(book_metadata.body_path)
        if not body_path.exists():
            return set()
        content = body_path.read_text(encoding="utf-8", errors="ignore")
        return TextNormalizer.normalize(content)

    @staticmethod
    def normalize(text: str) -> set[str]:
        normalized = IS_NOT_UNICODE_OR_NUMBER.sub(" ", text.lower())
        words = normalized.split()
        return {w for w in words if len(w) > 1}
