from mindsweeper.control.BookFeeder import BookFeeder
from mindsweeper.control.BookEventStorePublisher import BookEventStorePublisher

def main():
    for book_id in range(1342, 1400):
        print(f"Descargando libro ID: {book_id}...")
        book_event = BookFeeder.get_book_event(book_id)
        if book_event:
            BookEventStorePublisher.save(book_event)
            print(f"Libro {book_id} publicado con éxito.")
        else:
            print(f"Libro {book_id} descartado (no se pudo procesar).")

if __name__ == "__main__":
    main()

