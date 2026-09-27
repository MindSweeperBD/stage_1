import logging
import time
from pathlib import Path
from Index.MetaData import Metadata
from Index.MonolithicInvertedIndexBuilder import MonolithicInvertedIndexBuilder
from Index.HierarchicalInvertedIndexBuilder import HierarchicalInvertedIndexBuilder
from Index.MongoInvertedIndexBuilder import MongoInvertedIndexBuilder

log = logging.getLogger(__name__)

SAMPLE_QUERIES = ["project", "gutenberg", "adventure", "elizabeth", "darcy", "unknown_term"]


class IndexBenchmarkRunner:
    def __init__(self, output_dir: Path = Path("benchmark_datamart")):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        self.mono_path = self.output_dir / "inverted_index.json"
        self.hier_dir = self.output_dir / "hierarchical_index"

    def benchmark_all(self, books: list[Metadata]):
        if not books:
            log.warning("No hay libros disponibles para el benchmark de índices.")
            return

        print("\n==================== INVERTED INDEX BENCHMARK ====================")
        self.benchmark_monolithic(books)
        self.benchmark_hierarchical(books)
        self.benchmark_mongo(books)

    def benchmark_monolithic(self, books: list[Metadata]):
        log.info("--- [1/3] Benchmarking Monolithic JSON Index ---")
        mono = MonolithicInvertedIndexBuilder()

        # 1. Indexing speed
        start = time.perf_counter()
        mono.process_books(books)
        mono.save_inverted_index(self.mono_path)
        index_time = time.perf_counter() - start
        terms_count = mono.get_number_of_terms()

        # 2. Disk usage
        disk_bytes = self.mono_path.stat().st_size if self.mono_path.exists() else 0
        disk_kb = disk_bytes / 1024

        # 3. Query performance
        avg_query_ms = self._benchmark_queries(mono, SAMPLE_QUERIES)

        # 4. Incremental update (añadir el primer libro de nuevo como test de update)
        start_update = time.perf_counter()
        mono.process_book(books[0])
        mono.save_inverted_index(self.mono_path)
        update_ms = (time.perf_counter() - start_update) * 1000

        log.info(
            f"Monolithic JSON: {len(books)} libros ({terms_count} términos) en {index_time:.3f}s | "
            f"Disco: {disk_kb:.1f} KB | Query media: {avg_query_ms:.4f} ms | Update 1 libro: {update_ms:.2f} ms"
        )

    def benchmark_hierarchical(self, books: list[Metadata]):
        log.info("--- [2/3] Benchmarking Hierarchical Folder Index ---")
        hier = HierarchicalInvertedIndexBuilder(self.hier_dir)

        # 1. Indexing speed
        start = time.perf_counter()
        hier.process_books(books)
        hier.save()
        index_time = time.perf_counter() - start
        terms_count = hier.get_number_of_terms()

        # 2. Disk usage
        disk_bytes = self._get_dir_size(self.hier_dir)
        disk_kb = disk_bytes / 1024
        file_count = sum(1 for _ in self.hier_dir.rglob("*.txt"))

        # 3. Query performance (lee de disco cada archivo)
        avg_query_ms = self._benchmark_queries(hier, SAMPLE_QUERIES)

        # 4. Incremental update
        start_update = time.perf_counter()
        hier.process_book(books[0])
        hier.save()
        update_ms = (time.perf_counter() - start_update) * 1000

        log.info(
            f"Hierarchical TXT: {len(books)} libros ({terms_count} términos) en {index_time:.3f}s | "
            f"Disco: {disk_kb:.1f} KB ({file_count} archivos) | Query media: {avg_query_ms:.4f} ms | Update 1 libro: {update_ms:.2f} ms"
        )

    def benchmark_mongo(self, books: list[Metadata]):
        log.info("--- [3/3] Benchmarking MongoDB NoSQL Index ---")

        if not MongoInvertedIndexBuilder.is_server_available():
            log.warning("MongoDB NoSQL: Servidor no disponible en localhost:27017 (Omitido).")
            log.info("Para evaluar MongoDB, asegúrate de que el servicio mongod esté iniciado.")
            return

        mongo = MongoInvertedIndexBuilder()
        try:
            mongo.collection.drop()
            mongo.ensure_index()
            # 1. Indexing speed


            start = time.perf_counter()
            mongo.process_books(books)
            index_time = time.perf_counter() - start
            terms_count = mongo.get_number_of_terms()

            # 2. Disk / DB usage
            stats = mongo.db.command("collstats", "inverted_index")
            disk_bytes = stats.get("storageSize", stats.get("size", 0))
            disk_kb = disk_bytes / 1024

            # 3. Query performance
            avg_query_ms = self._benchmark_queries(mongo, SAMPLE_QUERIES)

            # 4. Incremental update
            start_update = time.perf_counter()
            mongo.process_book(books[0])
            update_ms = (time.perf_counter() - start_update) * 1000

            log.info(
                f"MongoDB NoSQL: {len(books)} libros ({terms_count} términos) en {index_time:.3f}s | "
                f"Storage: {disk_kb:.1f} KB | Query media: {avg_query_ms:.4f} ms | Update 1 libro: {update_ms:.2f} ms"
            )
        except Exception as e:
            log.error(f"Error evaluando MongoDB: {e}")
        finally:
            mongo.close()

    @staticmethod
    def _benchmark_queries(index, query_terms: list[str], repetitions: int = 100) -> float:
        start = time.perf_counter()
        for _ in range(repetitions):
            for term in query_terms:
                _ = index.get_postings(term)
        total_time = time.perf_counter() - start
        return (total_time / (repetitions * len(query_terms))) * 1000

    @staticmethod
    def _get_dir_size(path: Path) -> int:
        if not path.exists():
            return 0
        return sum(f.stat().st_size for f in path.rglob("*") if f.is_file())
