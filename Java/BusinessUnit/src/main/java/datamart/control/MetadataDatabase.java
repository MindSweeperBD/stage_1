package datamart.control;

import datamart.model.Metadata;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.List;

public class MetadataDatabase {

    private final String databaseUrl;

    public MetadataDatabase(String databasePath) {
        this.databaseUrl = "jdbc:sqlite:" + databasePath;
    }

    public void createDatabase() throws SQLException {
        try (Connection connection = DriverManager.getConnection(databaseUrl)) {
            String TABLE_FORMAT = """
                    CREATE TABLE IF NOT EXISTS books (
                        book_id INTEGER PRIMARY KEY,
                        title TEXT NOT NULL,
                        author TEXT NOT NULL,
                        language TEXT NOT NULL,
                        body_path TEXT NOT NULL
                    )
                    """;

            try (PreparedStatement statement = connection.prepareStatement(TABLE_FORMAT)) {
                statement.executeUpdate();
            }
            createIndexes(connection);
        }
    }

    private void createIndexes(Connection connection)
            throws SQLException {

        String authorIndex = """
                CREATE INDEX IF NOT EXISTS idx_books_author
                ON books(author)
                """;

        String titleIndex = """
                CREATE INDEX IF NOT EXISTS idx_books_title
                ON books(title)
                """;

        String languageIndex = """
                CREATE INDEX IF NOT EXISTS idx_books_language
                ON books(language)
                """;

        try (PreparedStatement statement = connection.prepareStatement(authorIndex)) {
            statement.executeUpdate();
        }
        try (PreparedStatement statement = connection.prepareStatement(titleIndex)) {
            statement.executeUpdate();
        }
        try (PreparedStatement statement = connection.prepareStatement(languageIndex)) {
            statement.executeUpdate();
        }
    }

    public void insertMetadata(List<Metadata> metadataList) throws SQLException {
        String sql = """
                INSERT OR REPLACE INTO books
                (book_id, title, author, language, body_path)
                VALUES (?, ?, ?, ?, ?)
                """;

        try (Connection connection = DriverManager.getConnection(databaseUrl);
             PreparedStatement statement = connection.prepareStatement(sql)) {
            uploadToDatabase(metadataList, connection, statement);
        }
    }

    private static void uploadToDatabase(List<Metadata> metadataList, Connection connection, PreparedStatement statement) throws SQLException {
        connection.setAutoCommit(false);

        for (Metadata bookMetadata : metadataList) {
            statement.setInt(1, bookMetadata.bookId());
            statement.setString(2, bookMetadata.title());
            statement.setString(3, bookMetadata.author());
            statement.setString(4, bookMetadata.language());
            statement.setString(5, bookMetadata.bodyPath());

            statement.addBatch();
        }
        statement.executeBatch();

        connection.commit();
    }
}
