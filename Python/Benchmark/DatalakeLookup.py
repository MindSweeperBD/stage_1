from pathlib import Path
from Downloader.Model.DatalakeLayout import DatalakeLayout

class DatalakeLookup:
    @staticmethod
    def find_book_part(layout: DatalakeLayout, book_id: str, part: str) -> Path | None:
        if layout == DatalakeLayout.TIME_BASED:
            root = Path("time_datalake")
            if not root.exists(): return None
            matches = list(root.glob(f"*/*/{book_id}.{part}.txt"))
            return matches[0] if matches else None

        elif layout == DatalakeLayout.BOOK_BASED:
            candidate = Path("book_datalake") / book_id / f"{part}.txt"
            return candidate if candidate.exists() else None

        elif layout == DatalakeLayout.BATCH_BASED:
            bid = int(book_id)
            start = (bid // 1000) * 1000
            batch = f"{start}-{start + 999}"
            candidate = Path("batch_datalake") / batch / f"{book_id}.{part}.txt"
            return candidate if candidate.exists() else None
