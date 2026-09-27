from Downloader.Control.BookFeeder import BookFeeder
from Downloader.Model.DatalakeLayout import DatalakeLayout

def main():
    layouts = [
        DatalakeLayout.TIME_BASED,
        DatalakeLayout.BOOK_BASED,
        DatalakeLayout.BATCH_BASED
    ]
    book_ids = [99, 177, 1342, 1610, 2700]

    for layout in layouts:
        print(f"Descargando en {layout.value}...")
        BookFeeder.save_books(book_ids, layout)
    print("Descargas completadas.")

if __name__ == "__main__":
    main()