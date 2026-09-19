import json
import logging
from datetime import datetime, timezone
from pathlib import Path
from eventstore.EventStore import EventStore
from eventstore.EventStoreSubscriber import EventStoreSubscriber

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
log = logging.getLogger(__name__)


class EventStoreBuilder(EventStore):

    def __init__(self, broker_url: str, client_id: str):
        self.broker_url = broker_url
        self.client_id = client_id
        self.base_dir = Path("eventstore")

    def store(self) -> None:
        subscriber = EventStoreSubscriber(self.broker_url, self.client_id)
        try:
            self._send_messages(subscriber)
        except Exception as e:
            log.error(f"Error in Event Store Builder: {e}")
        finally:
            subscriber.close()

    def _send_messages(self, subscriber: EventStoreSubscriber) -> None:
        subscriber.connect()
        subscriber.subscribe("GutenbergBooks", "EventStore_Gutenberg", self._handle_message)
        subscriber.wait_forever()

    def _handle_message(self, json_message: str) -> None:
        try:
            self._handle_event(json_message)
        except Exception as e:
            log.error(f"Error handling event for topic GutenbergBooks: {e}")

    def _handle_event(self, json_message: str) -> None:
        try:
            event = json.loads(json_message)
            ss = event.get("ss")
            ts = event.get("ts")
            # --- Añade estas 3 líneas para extraer los datos del evento ---
            book_id = event.get("bookId")
            header = event.get("header", "")
            body = event.get("body", "")

            # Obtener fecha y hora del timestamp (ts)
            dt = datetime.fromtimestamp(ts / 1000, tz=timezone.utc)
            date_str = dt.strftime("%Y%m%d")
            hour_str = dt.strftime("%H")

            # Ruta: datalake/YYYYMMDD/HH/
            lake_dir = Path("datalake") / date_str / hour_str
            lake_dir.mkdir(parents=True, exist_ok=True)

            # Guardar los dos archivos requeridos
            (lake_dir / f"{book_id}.body.txt").write_text(body, encoding="utf-8")
            (lake_dir / f"{book_id}.header.txt").write_text(header, encoding="utf-8")

        except Exception as e:
            log.error(f"Error handling event: {e}")

    def _resolve_event_file(self, ss: str, date_str: str) -> Path:
        path = self.base_dir / "GutenbergBooks" / ss
        path.mkdir(parents=True, exist_ok=True)
        return path / f"{date_str}.events"

    def write_event_to_file(self, file_path: Path, json_message: str) -> None:
        try:
            with open(file_path, "a", encoding="utf-8") as f:
                f.write(json_message + "\n")
            log.info(f"Event stored in: {file_path.resolve()}")
        except IOError as e:
            log.error(f"Error writing event to file: {e}")

