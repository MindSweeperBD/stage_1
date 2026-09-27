import logging
from pathlib import Path
from Index.DatalakeReader import DatalakeReader
from Index.MetaDataDataBase import MetadataDatabase
from Index.MonolithicInvertedIndexBuilder import MonolithicInvertedIndexBuilder
from Index.MetaData import Metadata

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
log = logging.getLogger(__name__)

DATALAKE_PATH = Path(__file__).resolve().parent.parent / "Benchmark" / "batch_datalake"  # o time_datalake según el que quieras indexar
DATAMART_PATH = Path("datamart")
METADATA_DATABASE = DATAMART_PATH / "metadata.db"
INVERTED_INDEX_FILE = DATAMART_PATH / "inverted_index.json"

def main():
    try:
        DATAMART_PATH.mkdir(parents=True, exist_ok=True)

        reader = DatalakeReader(DATALAKE_PATH)
        database = MetadataDatabase(str(METADATA_DATABASE))
        index_builder = MonolithicInvertedIndexBuilder()

        log.info("1. Leyendo metadatos del Datalake...")
        books = reader.read_metadata()
        log.info(f"Libros encontrados: {len(books)}")

        log.info("2. Guardando metadatos en SQLite...")
        database.create_database()
        database.insert_metadata(books)
        log.info(f"Metadatos guardados en: {METADATA_DATABASE}")

        log.info("3. Construyendo índice invertido...")
        index_builder.process_books(books)
        index_builder.save_inverted_index(INVERTED_INDEX_FILE)
        log.info(f"Índice invertido guardado en: {INVERTED_INDEX_FILE}")
        log.info(f"Número total de términos indexados: {index_builder.get_number_of_terms()}")

        log.info("¡Datamart construido con éxito!")
    except Exception as e:
        log.error(f"Error construyendo Datamart: {e}", exc_info=True)

if __name__ == "__main__":
    main()
