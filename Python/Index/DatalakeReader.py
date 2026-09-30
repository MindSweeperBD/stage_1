import logging
from pathlib import Path
from Index.HeaderParser import HeaderParser
from Index.MetaData import Metadata

log = logging.getLogger(__name__)

class DatalakeReader:
    def __init__(self, datalake_path: Path):
        self.datalake_path = datalake_path

    def read_metadata(self) -> list[Metadata]:
        metadata_list = []
        if not self.datalake_path.exists():
            return metadata_list

        for header_path in self.datalake_path.rglob("*.header.txt"):
            try:
                book_id = self._get_book_id(header_path)
                body_path = self._get_body_path(header_path, book_id)
                metadata = HeaderParser.parse(header_path, body_path, book_id)
                metadata_list.append(metadata)
            except Exception as e:
                log.error(f"Error procesando {header_path}: {e}")

        return metadata_list

    @staticmethod
    def _get_book_id(header_path: Path) -> int:
        file_name = header_path.name
        id_text = file_name.replace(".header.txt", "")
        return int(id_text)

    @staticmethod
    def _get_body_path(header_path: Path, book_id: int) -> Path:
        return header_path.parent / f"{book_id}.body.txt"
