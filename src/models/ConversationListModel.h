#pragma once

#include <QAbstractListModel>
#include "ConversationData.h"

class ConversationListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        ProviderNameRole,
        ModelNameRole,
        CreatedAtRole,
        UpdatedAtRole,
        MessageCountRole,
        PreviewRole,
        UpdatedAtFormattedRole
    };

    explicit ConversationListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setConversations(const QVector<ConversationData> &conversations);
    void addConversation(const ConversationData &conv);
    void removeConversation(const QString &id);
    void updateConversation(const QString &id, const ConversationData &data);
    int count() const;
    ConversationData at(int index) const;
    QVector<ConversationData> allConversations() const;

signals:
    void countChanged();

private:
    QVector<ConversationData> m_conversations;
    int findIndex(const QString &id) const;
};
