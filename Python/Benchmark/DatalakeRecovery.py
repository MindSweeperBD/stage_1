from pathlib import Path
from Downloader.Model.DatalakeLayout import DatalakeLayout

class DatalakeRecovery:
    def __init__(self, layout: DatalakeLayout, timestamps: dict, book_ids: list[int]):
        self.layout = layout
        self.timestamps = timestamps
        self.book_ids = book_ids
        self.header_count = {i: 0 for i in book_ids}
        self.body_count = {i: 0 for i in book_ids}

    def verify_recovery(self) -> bool:
        if self.layout == DatalakeLayout.TIME_BASED:
            root = Path("time_datalake")
            for bid in self.book_ids:
                if bid in self.timestamps:
                    d, h = self.timestamps[bid]
                    folder = root / d / h
                    if folder.exists():
                        for f in folder.glob(f"{bid}.*.txt"):
                            parts = f.name.split(".")
                            self._is_in_datalake(bid, parts[1])

        elif self.layout == DatalakeLayout.BOOK_BASED:
            root = Path("book_datalake")
            for f in root.glob("*/*.txt"):
                bid = int(f.parent.name)
                part = f.stem
                self._is_in_datalake(bid, part)

        elif self.layout == DatalakeLayout.BATCH_BASED:
            root = Path("batch_datalake")
            for f in root.glob("*/*.txt"):
                parts = f.name.split(".")
                bid = int(parts[0])
                part = parts[1]
                self._is_in_datalake(bid, part)

        return all(self.header_count[i] == 1 and self.body_count[i] == 1 for i in self.book_ids)

    def _is_in_datalake(self, book_id: int, type_: str):
        if book_id not in self.header_count: return
        if type_ == "header": self.header_count[book_id] += 1
        elif type_ == "body": self.body_count[book_id] += 1

