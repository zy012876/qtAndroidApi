#include "MessageListModel.h"

MessageListModel::MessageListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int MessageListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_messages.size();
}

QVariant MessageListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_messages.size())
        return {};

    const auto &msg = m_messages.at(index.row());
    switch (role) {
    case IdRole: return msg.id;
    case RoleRole: return msg.role;
    case ContentRole: return msg.content;
    case ProviderNameRole: return msg.providerName;
    case ModelNameRole: return msg.modelName;
    case TimestampRole: return msg.timestamp;
    case IsStreamingRole: return msg.isStreaming;
    case TimestampFormattedRole: return msg.timestamp.toString("h:mm AP");
    default: return {};
    }
}

QHash<int, QByteArray> MessageListModel::roleNames() const
{
    return {
        {IdRole, "msgId"},
        {RoleRole, "msgRole"},
        {ContentRole, "msgContent"},
        {ProviderNameRole, "msgProviderName"},
        {ModelNameRole, "msgModelName"},
        {TimestampRole, "msgTimestamp"},
        {IsStreamingRole, "msgIsStreaming"},
        {TimestampFormattedRole, "msgTimestampFormatted"}
    };
}

void MessageListModel::appendMessage(const MessageData &msg)
{
    int row = m_messages.size();
    beginInsertRows(QModelIndex(), row, row);
    m_messages.append(msg);
    endInsertRows();
    emit countChanged();
}

void MessageListModel::prependMessages(const QVector<MessageData> &messages)
{
    if (messages.isEmpty()) return;
    beginInsertRows(QModelIndex(), 0, messages.size() - 1);
    m_messages = messages + m_messages;
    endInsertRows();
    emit countChanged();
}

void MessageListModel::updateLastAssistantMessage(const QString &contentChunk)
{
    if (m_messages.isEmpty()) return;
    auto &last = m_messages.last();
    last.content += contentChunk;
    int row = m_messages.size() - 1;
    emit dataChanged(index(row), index(row), {ContentRole});
}

void MessageListModel::finalizeLastAssistantMessage(const QString &fullContent,
                                                      const QString &modelName)
{
    if (m_messages.isEmpty()) return;
    auto &last = m_messages.last();
    last.content = fullContent;
    last.modelName = modelName;
    last.isStreaming = false;
    int row = m_messages.size() - 1;
    emit dataChanged(index(row), index(row), {ContentRole, ModelNameRole, IsStreamingRole});
}

void MessageListModel::clear()
{
    if (m_messages.isEmpty()) return;
    beginRemoveRows(QModelIndex(), 0, m_messages.size() - 1);
    m_messages.clear();
    endRemoveRows();
    emit countChanged();
}

void MessageListModel::setMessages(const QVector<MessageData> &messages)
{
    beginResetModel();
    m_messages = messages;
    endResetModel();
    emit countChanged();
}

const MessageData &MessageListModel::at(int index) const
{
    return m_messages.at(index);
}

QVector<MessageData> MessageListModel::allMessages() const
{
    return m_messages;
}

int MessageListModel::count() const
{
    return m_messages.size();
}

qint64 MessageListModel::lastMessageId() const
{
    if (m_messages.isEmpty()) return -1;
    return m_messages.last().id;
}
