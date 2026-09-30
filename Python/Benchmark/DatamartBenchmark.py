import os
import gc
import tempfile
import time
import statistics
from Index.MetaData import Metadata
from Index.MetaDataDataBase import MetadataDatabase

BOOK_SIZES = [100, 1000, 5000, 10000, 25000, 50000]
ITERATIONS = 5


def generate_synthetic_metadata(n: int) -> list[Metadata]:
    return [
        Metadata(
            book_id=i,
            title=f"Book {i}",
            author=f"Author {i % 100}",
            language="English",
            body_path=f"books/{i}.body.txt"
        )
        for i in range(1, n + 1)
    ]


def safe_remove(path: str):
    gc.collect()
    try:
        if os.path.exists(path):
            os.remove(path)
    except PermissionError:
        time.sleep(0.05)
        try:
            if os.path.exists(path):
                os.remove(path)
        except Exception:
            pass


def run_benchmark():
    print(f"{'Benchmark':<35} {'(numberOfBooks)':>15}  {'Mode':>4}  {'Cnt':>3}     {'Score':>10}  {'Error':>8}   {'Units'}")

    results = []

    for n in BOOK_SIZES:
        metadata_list = generate_synthetic_metadata(n)


        insertion_times = []
        for _ in range(ITERATIONS):
            fd, temp_db = tempfile.mkstemp(suffix=".db")
            os.close(fd)
            try:
                db = MetadataDatabase(temp_db)
                db.create_database()
                start = time.perf_counter()
                db.insert_metadata(metadata_list)
                elapsed_ms = (time.perf_counter() - start) * 1000
                insertion_times.append(elapsed_ms)
            finally:
                safe_remove(temp_db)

        score_ins = statistics.mean(insertion_times)
        err_ins = statistics.stdev(insertion_times) if len(insertion_times) > 1 else 0.0
        results.append(("DatamartBenchmark.insertionSpeed", n, score_ins, err_ins))

        fd, base_db = tempfile.mkstemp(suffix=".db")
        os.close(fd)
        try:
            db = MetadataDatabase(base_db)
            db.create_database()
            db.insert_metadata(metadata_list)

            author_times = []
            for _ in range(ITERATIONS):
                start = time.perf_counter()
                _ = db.find_by_author("Author 99")
                elapsed_ms = (time.perf_counter() - start) * 1000
                author_times.append(elapsed_ms)

            score_author = statistics.mean(author_times)
            err_author = statistics.stdev(author_times) if len(author_times) > 1 else 0.0
            results.append(("DatamartBenchmark.queryByAuthor", n, score_author, err_author))


            id_times = []
            target_id = n // 2
            for _ in range(ITERATIONS):
                start = time.perf_counter()
                _ = db.find_body_path_by_id(target_id)
                elapsed_ms = (time.perf_counter() - start) * 1000
                id_times.append(elapsed_ms)

            score_id = statistics.mean(id_times)
            err_id = statistics.stdev(id_times) if len(id_times) > 1 else 0.0
            results.append(("DatamartBenchmark.queryById", n, score_id, err_id))

        finally:
            safe_remove(base_db)

    bench_names = [
        "DatamartBenchmark.insertionSpeed",
        "DatamartBenchmark.queryByAuthor",
        "DatamartBenchmark.queryById"
    ]
    for bname in bench_names:
        for name, n, score, err in results:
            if name == bname:
                score_str = f"{score:10.3f}".replace(".", ",")
                err_str = f"{err:8.3f}".replace(".", ",")
                print(f"{name:<35} {n:>15}  avgt    5    {score_str} ± {err_str}  ms/op")


if __name__ == "__main__":
    run_benchmark()