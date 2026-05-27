#pragma once

#include <QAbstractListModel>
#include "MessageData.h"

class MessageListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        RoleRole,
        ContentRole,
        ProviderNameRole,
        ModelNameRole,
        TimestampRole,
        IsStreamingRole,
        TimestampFormattedRole
    };

    explicit MessageListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void appendMessage(const MessageData &msg);
    void prependMessages(const QVector<MessageData> &messages);
    void updateLastAssistantMessage(const QString &contentChunk);
    void finalizeLastAssistantMessage(const QString &fullContent, const QString &modelName);
    void clear();
    void setMessages(const QVector<MessageData> &messages);

    const MessageData &at(int index) const;
    QVector<MessageData> allMessages() const;
    int count() const;
    qint64 lastMessageId() const;

signals:
    void countChanged();

private:
    QVector<MessageData> m_messages;
};
