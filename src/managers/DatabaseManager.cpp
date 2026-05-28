#include "DatabaseManager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QUuid>

DatabaseManager::DatabaseManager(const QString &dbPath, QObject *parent)
    : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen())
        m_db.close();
}

bool DatabaseManager::initialize()
{
    if (!m_db.open()) {
        qCritical() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);

    query.exec("PRAGMA foreign_keys = ON");

    // Schema version tracking
    query.exec("CREATE TABLE IF NOT EXISTS schema_version (version INTEGER)");
    int version = 0;
    if (query.exec("SELECT version FROM schema_version")) {
        if (query.next())
            version = query.value(0).toInt();
    }

    // Migrations: each step runs only if version < step number
    if (version < 1) {
        if (!query.exec(
            "CREATE TABLE IF NOT EXISTS conversations ("
            "  id TEXT PRIMARY KEY,"
            "  title TEXT NOT NULL DEFAULT 'New Chat',"
            "  provider_name TEXT NOT NULL,"
            "  model_name TEXT,"
            "  created_at TEXT NOT NULL DEFAULT (datetime('now')),"
            "  updated_at TEXT NOT NULL DEFAULT (datetime('now')),"
            "  message_count INTEGER NOT NULL DEFAULT 0,"
            "  preview TEXT"
            ")")) {
            qCritical() << "Failed to create conversations table:" << query.lastError().text();
            return false;
        }

        if (!query.exec(
            "CREATE TABLE IF NOT EXISTS messages ("
            "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  conversation_id TEXT NOT NULL,"
            "  role TEXT NOT NULL CHECK(role IN ('user', 'assistant', 'system')),"
            "  content TEXT NOT NULL,"
            "  provider_name TEXT,"
            "  model_name TEXT,"
            "  timestamp TEXT NOT NULL DEFAULT (datetime('now')),"
            "  FOREIGN KEY (conversation_id) REFERENCES conversations(id) ON DELETE CASCADE"
            ")")) {
            qCritical() << "Failed to create messages table:" << query.lastError().text();
            return false;
        }

        query.exec(
            "CREATE INDEX IF NOT EXISTS idx_messages_conv ON messages(conversation_id, timestamp)");

        // Update schema version
        if (version == 0) {
            query.exec("INSERT INTO schema_version VALUES (1)");
        } else {
            query.exec("UPDATE schema_version SET version = 1");
        }
    }

    return true;
}

QString DatabaseManager::createConversation(const QString &title, const QString &provider,
                                             const QString &model)
{
    QString id = QUuid::createUuid().toString(QUuid::WithoutBraces);

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO conversations (id, title, provider_name, model_name) "
        "VALUES (?, ?, ?, ?)");
    query.addBindValue(id);
    query.addBindValue(title.isEmpty() ? "New Chat" : title);
    query.addBindValue(provider);
    query.addBindValue(model);

    if (!query.exec()) {
        qWarning() << "Failed to create conversation:" << query.lastError().text();
        return {};
    }

    return id;
}

QVector<ConversationData> DatabaseManager::loadAllConversations()
{
    QVector<ConversationData> result;
    QSqlQuery query(m_db);

    query.exec(
        "SELECT id, title, provider_name, model_name, created_at, updated_at, "
        "message_count, preview FROM conversations ORDER BY updated_at DESC");

    while (query.next()) {
        ConversationData conv;
        conv.id = query.value(0).toString();
        conv.title = query.value(1).toString();
        conv.providerName = query.value(2).toString();
        conv.modelName = query.value(3).toString();
        conv.createdAt = QDateTime::fromString(query.value(4).toString(), Qt::ISODate);
        conv.updatedAt = QDateTime::fromString(query.value(5).toString(), Qt::ISODate);
        conv.messageCount = query.value(6).toInt();
        conv.preview = query.value(7).toString();
        result.append(conv);
    }

    return result;
}

ConversationData DatabaseManager::loadConversation(const QString &id)
{
    ConversationData conv;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, title, provider_name, model_name, created_at, updated_at, "
        "message_count, preview FROM conversations WHERE id = ?");
    query.addBindValue(id);

    if (query.exec() && query.next()) {
        conv.id = query.value(0).toString();
        conv.title = query.value(1).toString();
        conv.providerName = query.value(2).toString();
        conv.modelName = query.value(3).toString();
        conv.createdAt = QDateTime::fromString(query.value(4).toString(), Qt::ISODate);
        conv.updatedAt = QDateTime::fromString(query.value(5).toString(), Qt::ISODate);
        conv.messageCount = query.value(6).toInt();
        conv.preview = query.value(7).toString();
    }

    return conv;
}

void DatabaseManager::updateConversation(const QString &id, const QString &title,
                                          const QString &preview, int messageCountDelta)
{
    QSqlQuery query(m_db);

    QStringList sets;
    sets << "updated_at = datetime('now')";

    if (!title.isEmpty()) {
        sets << "title = ?";
    }
    if (!preview.isEmpty()) {
        sets << "preview = ?";
    }
    if (messageCountDelta != 0) {
        sets << "message_count = message_count + ?";
    }

    QString sql = "UPDATE conversations SET " + sets.join(", ") + " WHERE id = ?";
    query.prepare(sql);

    if (!title.isEmpty()) query.addBindValue(title);
    if (!preview.isEmpty()) query.addBindValue(preview);
    if (messageCountDelta != 0) query.addBindValue(messageCountDelta);
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "Failed to update conversation:" << query.lastError().text();
    }
}

void DatabaseManager::deleteConversation(const QString &id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM conversations WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "Failed to delete conversation:" << query.lastError().text();
    }
}

qint64 DatabaseManager::insertMessage(const MessageData &msg)
{
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO messages (conversation_id, role, content, provider_name, model_name) "
        "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(msg.conversationId);
    query.addBindValue(msg.role);
    query.addBindValue(msg.content);
    query.addBindValue(msg.providerName);
    query.addBindValue(msg.modelName);

    if (!query.exec()) {
        qWarning() << "Failed to insert message:" << query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toLongLong();
}

QVector<MessageData> DatabaseManager::loadMessages(const QString &conversationId,
                                                    int limit, int offset)
{
    QVector<MessageData> result;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, conversation_id, role, content, provider_name, model_name, timestamp "
        "FROM messages WHERE conversation_id = ? "
        "ORDER BY timestamp ASC LIMIT ? OFFSET ?");
    query.addBindValue(conversationId);
    query.addBindValue(limit);
    query.addBindValue(offset);

    if (!query.exec()) {
        qWarning() << "Failed to load messages:" << query.lastError().text();
        return result;
    }

    while (query.next()) {
        MessageData msg;
        msg.id = query.value(0).toLongLong();
        msg.conversationId = query.value(1).toString();
        msg.role = query.value(2).toString();
        msg.content = query.value(3).toString();
        msg.providerName = query.value(4).toString();
        msg.modelName = query.value(5).toString();
        msg.timestamp = QDateTime::fromString(query.value(6).toString(), Qt::ISODate);
        result.append(msg);
    }

    return result;
}

void DatabaseManager::updateMessageContent(qint64 messageId, const QString &content,
                                            const QString &modelName)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE messages SET content = ?, model_name = ? WHERE id = ?");
    query.addBindValue(content);
    query.addBindValue(modelName);
    query.addBindValue(messageId);

    if (!query.exec()) {
        qWarning() << "Failed to update message:" << query.lastError().text();
    }
}

void DatabaseManager::deleteMessages(const QString &conversationId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM messages WHERE conversation_id = ?");
    query.addBindValue(conversationId);

    if (!query.exec()) {
        qWarning() << "Failed to delete messages:" << query.lastError().text();
    }
}
