package datamart.control;

import com.mongodb.client.MongoClient;
import com.mongodb.client.MongoClients;
import com.mongodb.client.MongoCollection;
import com.mongodb.client.MongoDatabase;
import com.mongodb.client.model.IndexOptions;
import datamart.model.Metadata;
import org.bson.Document;

import java.io.IOException;
import java.util.List;

import static datamart.control.TextNormalizer.getWordsList;

public class MongoInvertedIndexBuilder {

    private final MongoClient mongoClient;
    private final MongoCollection<Document> collection;

    public MongoInvertedIndexBuilder(String connectionString) {
        mongoClient = MongoClients.create(connectionString);
        MongoDatabase database = mongoClient.getDatabase("datamart");
        collection = database.getCollection("inverted_index");

        collection.createIndex(
                new Document("term", 1),
                new IndexOptions().unique(true)
        );
    }

    public void processBooks(List<Metadata> books) throws IOException {
        for (Metadata bookMetadata : books) {
            for (String word : getWordsList(bookMetadata)) {
                addWordToMongo(bookMetadata, word);
            }
        }
    }

    private void addWordToMongo(Metadata bookMetadata, String word) {
        if (isWordInDocument(word)) {
            updateTerm(bookMetadata, word);
        } else {
            addTerm(bookMetadata, word);
        }
    }

    private boolean isWordInDocument(String word) {
        Document exists = collection.find(
                new Document("term", word)
        ).first();
        return exists != null;
    }

    private void updateTerm(Metadata bookMetadata, String word) {
        collection.updateOne(
                new Document("term", word),
                new Document(
                        "$addToSet",
                        new Document(
                                "postings",
                                bookMetadata.bookId()
                        )
                )
        );
    }

    private void addTerm(Metadata bookMetadata, String word) {
        Document document = new Document()
                .append("term", word)
                .append(
                        "postings",
                        List.of(bookMetadata.bookId())
                );

        collection.insertOne(document);
    }

    public void close() {
        mongoClient.close();
    }
}
