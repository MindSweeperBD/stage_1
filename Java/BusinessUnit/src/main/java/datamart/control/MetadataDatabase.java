package datamart.control;

import datamart.model.Metadata;

import java.sql.*;
import java.util.ArrayList;
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

    public List<Metadata> findByAuthor(String author) throws SQLException {
        String sql = """
            SELECT book_id, title, author, language, body_path
            FROM books
            WHERE author = ?
            """;

        List<Metadata> result = new ArrayList<>();
        searchAuthorInDatabase(author, sql, result);
        return result;
    }

    private void searchAuthorInDatabase(String author, String sql, List<Metadata> result) throws SQLException {
        try (Connection connection = DriverManager.getConnection(databaseUrl);
             PreparedStatement statement = connection.prepareStatement(sql)) {

            statement.setString(1, author);

            try (ResultSet resultSet = statement.executeQuery()) {
                addAllResults(resultSet, result);
            }
        }
    }

    private static void addAllResults(ResultSet resultSet, List<Metadata> result) throws SQLException {
        while (resultSet.next()) {
            result.add(
                    new Metadata(
                            resultSet.getInt("book_id"),
                            resultSet.getString("title"),
                            resultSet.getString("author"),
                            resultSet.getString("language"),
                            resultSet.getString("body_path")
                    )
            );
        }
    }

    public String findBodyPathById(int bookId) throws SQLException {
        String sql = """
            SELECT body_path
            FROM books
            WHERE book_id = ?
            """;

        return getBodyPathInDatabase(bookId, sql);
    }

    private String getBodyPathInDatabase(int bookId, String sql) throws SQLException {
        try (Connection connection = DriverManager.getConnection(databaseUrl);
             PreparedStatement statement = connection.prepareStatement(sql)) {

            statement.setInt(1, bookId);

            try (ResultSet resultSet = statement.executeQuery()) {
                if (resultSet.next()) {
                    return resultSet.getString("body_path");
                }
                return null;
            }
        }
    }
}
