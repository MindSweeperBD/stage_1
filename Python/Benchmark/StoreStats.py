from dataclasses import dataclass

@dataclass(frozen=True)
class StorageStats:
    directories: int
    files: int
    auxiliary: int
