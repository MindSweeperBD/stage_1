import re
from pathlib import Path
from Index.MetaData import Metadata

TITLE_PATTERN = re.compile(r"^Title:\s*(.+)$", re.MULTILINE)
AUTHOR_PATTERN = re.compile(r"^Author:\s*(.+)$", re.MULTILINE)
LANGUAGE_PATTERN = re.compile(r"^Language:\s*(.+)$", re.MULTILINE)

class HeaderParser:
    @staticmethod
    def parse(header_path: Path, body_path: Path, book_id: int) -> Metadata:
        content = header_path.read_text(encoding="utf-8", errors="ignore")
        title = HeaderParser._extract(TITLE_PATTERN, content)
        author = HeaderParser._extract(AUTHOR_PATTERN, content)
        language = HeaderParser._extract(LANGUAGE_PATTERN, content)

        return Metadata(
            book_id=book_id,
            title=title,
            author=author,
            language=language,
            body_path=str(body_path)
        )

    @staticmethod
    def _extract(pattern: re.Pattern, content: str) -> str:
        match = pattern.search(content)
        return match.group(1).strip() if match else ""

