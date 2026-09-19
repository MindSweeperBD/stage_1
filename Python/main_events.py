import logging
from eventstore.EventStoreBuilder import EventStoreBuilder

logging.basicConfig(level=logging.INFO, format="%(asctime)s [%(levelname)s] %(message)s")
log = logging.getLogger(__name__)

def main():
    broker_url = "localhost:61613"
    client_id = "EventStoreBuilder"

    log.info("Starting Event Store Builder...")
    builder = EventStoreBuilder(broker_url, client_id)
    builder.store()

if __name__ == "__main__":
    main()
