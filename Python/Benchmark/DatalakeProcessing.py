from pathlib import Path
from Downloader.Model.DatalakeLayout import DatalakeLayout

class DatalakeProcessing:
    @staticmethod
    def scan_book_ids(layout: DatalakeLayout) -> set[str]:
        ids = set()
        if layout == DatalakeLayout.TIME_BASED:
            p = Path("time_datalake")
            if p.exists():
                for f in p.glob("*/*/*.txt"):
                    ids.add(f.name.split(".")[0])
        elif layout == DatalakeLayout.BOOK_BASED:
            p = Path("book_datalake")
            if p.exists():
                for d in p.iterdir():
                    if d.is_dir(): ids.add(d.name)
        elif layout == DatalakeLayout.BATCH_BASED:
            p = Path("batch_datalake")
            if p.exists():
                for f in p.glob("*/*.txt"):
                    ids.add(f.name.split(".")[0])
        return ids
