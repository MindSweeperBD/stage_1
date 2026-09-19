import logging
import time
from typing import Callable
import stomp

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
log = logging.getLogger(__name__)


class _StompListener(stomp.ConnectionListener):
    """Listener interno que recibe el mensaje de ActiveMQ y se lo pasa al callback."""
    def __init__(self, callback: Callable[[str], None]):
        self.callback = callback

    def on_message(self, frame):
        self.callback(frame.body)


class EventStoreSubscriber:

    def __init__(self, broker_url: str, client_id: str):
        self.broker_url = broker_url  # ej: "localhost:61613"
        self.client_id = client_id
        self.conn = None

    def connect(self) -> None:
        host, port = self._parse_broker_url(self.broker_url)
        self.conn = stomp.Connection([(host, port)], auto_content_length=False)
        self.conn.connect(client_id=self.client_id, wait=True)
        log.info(f"Connected to broker at {host}:{port} with clientID {self.client_id}")

    def subscribe(self, topic_name: str, subscription_name: str, callback: Callable[[str], None]) -> None:
        # Registramos el listener con la función que procesará el JSON
        self.conn.set_listener("event_store_listener", _StompListener(callback))

        # En STOMP, una suscripción durable a un Topic usa estos headers
        headers = {
            "activemq.subscriptionName": subscription_name,
            "id": subscription_name
        }
        self.conn.subscribe(
            destination=f"/topic/{topic_name}",
            id=subscription_name,
            ack="auto",
            headers=headers
        )
        log.info(f"Subscribed to topic: {topic_name}")

    def wait_forever(self) -> None:
        log.info("Event Store Subscriber running...")
        try:
            while True:
                time.sleep(1)
        except KeyboardInterrupt:
            log.info("Stopping subscriber...")

    def close(self) -> None:
        try:
            if self.conn and self.conn.is_connected():
                self.conn.disconnect()
        except Exception:
            pass

    @staticmethod
    def _parse_broker_url(broker_url: str) -> tuple[str, int]:
        # Extrae host y puerto (por defecto localhost:61613 para STOMP)
        clean_url = broker_url.replace("tcp://", "").replace("failover:(", "").replace(")", "")
        parts = clean_url.split(":")
        host = parts[0] if parts[0] else "localhost"
        port = int(parts[1]) if len(parts) > 1 else 61613
        # Si le pasan el puerto 61616 de Java, lo mapeamos al 61613 de STOMP
        if port == 61616:
            port = 61613
        return host, port
