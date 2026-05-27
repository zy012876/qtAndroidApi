#pragma once

#include <QObject>
#include <QJsonArray>

class ProviderFactory;
class DatabaseManager;
class SettingsManager;
class MessageListModel;
class ConversationListModel;
class AiProviderBase;

class ChatManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MessageListModel *currentMessages READ currentMessages CONSTANT)
    Q_PROPERTY(ConversationListModel *conversations READ conversations CONSTANT)
    Q_PROPERTY(QString currentConversationId READ currentConversationId
               NOTIFY currentConversationChanged)
    Q_PROPERTY(QString currentProviderId READ currentProviderId
               WRITE setCurrentProviderId NOTIFY currentProviderIdChanged)
    Q_PROPERTY(QString currentModel READ currentModel
               WRITE setCurrentModel NOTIFY currentModelChanged)
    Q_PROPERTY(bool isStreaming READ isStreaming NOTIFY isStreamingChanged)
    Q_PROPERTY(bool hasActiveConversation READ hasActiveConversation
               NOTIFY currentConversationChanged)
    Q_PROPERTY(QStringList registeredProviders READ registeredProviders
               NOTIFY registeredProvidersChanged)

public:
    explicit ChatManager(ProviderFactory *providerFactory,
                         DatabaseManager *databaseManager,
                         SettingsManager *settingsManager,
                         QObject *parent = nullptr);

    MessageListModel *currentMessages() const;
    ConversationListModel *conversations() const;
    QString currentConversationId() const;
    QString currentProviderId() const;
    QString currentModel() const;
    bool isStreaming() const;
    bool hasActiveConversation() const;
    QStringList registeredProviders() const;

    Q_INVOKABLE void sendMessage(const QString &text);
    Q_INVOKABLE void cancelCurrentRequest();
    Q_INVOKABLE void createNewConversation(const QString &providerId = QString());
    Q_INVOKABLE void switchConversation(const QString &conversationId);
    Q_INVOKABLE void deleteConversation(const QString &conversationId);
    Q_INVOKABLE void renameConversation(const QString &conversationId, const QString &title);
    Q_INVOKABLE void setCurrentProviderId(const QString &providerId);
    Q_INVOKABLE void setCurrentModel(const QString &modelName);
    Q_INVOKABLE QStringList availableModels(const QString &providerId) const;
    Q_INVOKABLE void loadLastSession();

signals:
    void currentConversationChanged();
    void currentProviderIdChanged();
    void currentModelChanged();
    void isStreamingChanged();
    void errorOccurred(const QString &title, const QString &message);
    void registeredProvidersChanged();

private slots:
    void onResponseChunk(const QString &chunk, const QString &conversationId);
    void onResponseComplete(const QString &fullResponse, const QString &conversationId,
                            const QVariantMap &usage);
    void onProviderError(const QString &errorMessage, int httpStatusCode);

private:
    void persistMessage(const QJsonObject &msg);
    void refreshConversations();
    void ensureConversation();

    ProviderFactory *m_providerFactory;
    DatabaseManager *m_databaseManager;
    SettingsManager *m_settingsManager;
    MessageListModel *m_currentMessages;
    ConversationListModel *m_conversations;
    QString m_currentConversationId;
    QString m_currentProviderId;
    QString m_currentModel;
    bool m_isStreaming = false;
};
