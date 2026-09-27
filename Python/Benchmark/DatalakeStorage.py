from pathlib import Path
from Downloader.Model.DatalakeLayout import DatalakeLayout
from Benchmark.StoreStats import StorageStats

class DatalakeStorage:
    def __init__(self, layout: DatalakeLayout):
        self.layout = layout

    def measure(self) -> StorageStats:
        folder_name = {
            DatalakeLayout.TIME_BASED: "time_datalake",
            DatalakeLayout.BOOK_BASED: "book_datalake",
            DatalakeLayout.BATCH_BASED: "batch_datalake"
        }[self.layout]

        root = Path(folder_name)
        dirs = files = aux = 0
        if root.exists():
            for p in root.rglob("*"):
                if p.is_dir():
                    dirs += 1
                elif p.is_file():
                    files += 1
                    if p.suffix in [".idx", ".meta", ".checkpoint", ".log"]:
                        aux += 1
        return StorageStats(directories=dirs, files=files, auxiliary=aux)
