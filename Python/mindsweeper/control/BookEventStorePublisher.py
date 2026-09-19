import json
import logging
import stomp
from mindsweeper.model.BookEvent import BookEvent

# Configuración del Logger (equivalente a SLF4J en Java)
logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
log = logging.getLogger(__name__)

# ActiveMQ expone STOMP por defecto en el puerto 61613
ACTIVEMQ_HOST = "localhost"
ACTIVEMQ_PORT = 61613
TOPIC = "/topic/GutenbergBooks"


class BookEventStorePublisher:

    @staticmethod
    def save(event: BookEvent) -> None:
        json_message = json.dumps(event.to_dict())  # json.dumps equivale a Gson.toJson()
        BookEventStorePublisher._publish(event, json_message)

    @staticmethod
    def _publish(event: BookEvent, json_message: str) -> None:
        conn = None
        try:
            conn = stomp.Connection([(ACTIVEMQ_HOST, ACTIVEMQ_PORT)])
            conn.connect(wait=True)

            conn.send(body=json_message, destination=TOPIC)
            log.info(f"Book {event.bookId} sent to topic {TOPIC}")

        except Exception as e:
            log.error(f"Error publishing BookEvent to ActiveMQ: {e}")
        finally:
            if conn and conn.is_connected():
                conn.disconnect()
