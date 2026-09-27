from enum import Enum

class DatalakeLayout(Enum):
    TIME_BASED = "TIME_BASED"
    BOOK_BASED = "BOOK_BASED"
    BATCH_BASED = "BATCH_BASED"
