#include "ConversationListModel.h"

ConversationListModel::ConversationListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ConversationListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_conversations.size();
}

QVariant ConversationListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_conversations.size())
        return {};

    const auto &conv = m_conversations.at(index.row());
    switch (role) {
    case IdRole: return conv.id;
    case TitleRole: return conv.title;
    case ProviderNameRole: return conv.providerName;
    case ModelNameRole: return conv.modelName;
    case CreatedAtRole: return conv.createdAt;
    case UpdatedAtRole: return conv.updatedAt;
    case MessageCountRole: return conv.messageCount;
    case PreviewRole: return conv.preview;
    case UpdatedAtFormattedRole: return conv.updatedAt.toString("yyyy-MM-dd h:mm");
    default: return {};
    }
}

QHash<int, QByteArray> ConversationListModel::roleNames() const
{
    return {
        {IdRole, "convId"},
        {TitleRole, "convTitle"},
        {ProviderNameRole, "convProviderName"},
        {ModelNameRole, "convModelName"},
        {CreatedAtRole, "convCreatedAt"},
        {UpdatedAtRole, "convUpdatedAt"},
        {MessageCountRole, "convMessageCount"},
        {PreviewRole, "convPreview"},
        {UpdatedAtFormattedRole, "convUpdatedAtFormatted"}
    };
}

void ConversationListModel::setConversations(const QVector<ConversationData> &conversations)
{
    beginResetModel();
    m_conversations = conversations;
    endResetModel();
    emit countChanged();
}

void ConversationListModel::addConversation(const ConversationData &conv)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_conversations.prepend(conv);
    endInsertRows();
    emit countChanged();
}

void ConversationListModel::removeConversation(const QString &id)
{
    int idx = findIndex(id);
    if (idx < 0) return;
    beginRemoveRows(QModelIndex(), idx, idx);
    m_conversations.removeAt(idx);
    endRemoveRows();
    emit countChanged();
}

void ConversationListModel::updateConversation(const QString &id, const ConversationData &data)
{
    int idx = findIndex(id);
    if (idx < 0) return;
    m_conversations[idx] = data;
    m_conversations[idx].id = id;
    emit dataChanged(index(idx), index(idx));
}

int ConversationListModel::count() const
{
    return m_conversations.size();
}

ConversationData ConversationListModel::at(int index) const
{
    return m_conversations.at(index);
}

QVector<ConversationData> ConversationListModel::allConversations() const
{
    return m_conversations;
}

int ConversationListModel::findIndex(const QString &id) const
{
    for (int i = 0; i < m_conversations.size(); ++i) {
        if (m_conversations[i].id == id)
            return i;
    }
    return -1;
}
