#pragma once

#include <QObject>
#include <QSqlDatabase>
#include "models/MessageData.h"
#include "models/ConversationData.h"

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(const QString &dbPath, QObject *parent = nullptr);
    ~DatabaseManager();

    bool initialize();

    QString createConversation(const QString &title, const QString &provider,
                                const QString &model);
    QVector<ConversationData> loadAllConversations();
    ConversationData loadConversation(const QString &id);
    void updateConversation(const QString &id, const QString &title = QString(),
                            const QString &preview = QString(), int messageCountDelta = 0);
    void deleteConversation(const QString &id);

    qint64 insertMessage(const MessageData &msg);
    QVector<MessageData> loadMessages(const QString &conversationId, int limit = 100,
                                       int offset = 0);
    void updateMessageContent(qint64 messageId, const QString &content,
                               const QString &modelName);
    void deleteMessages(const QString &conversationId);

private:
    QSqlDatabase m_db;
};
