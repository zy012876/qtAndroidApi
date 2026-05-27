#include "ChatManager.h"
#include "DatabaseManager.h"
#include "SettingsManager.h"
#include "models/MessageListModel.h"
#include "models/ConversationListModel.h"
#include "providers/ProviderFactory.h"
#include "providers/AiProviderBase.h"

ChatManager::ChatManager(ProviderFactory *providerFactory,
                           DatabaseManager *databaseManager,
                           SettingsManager *settingsManager,
                           QObject *parent)
    : QObject(parent)
    , m_providerFactory(providerFactory)
    , m_databaseManager(databaseManager)
    , m_settingsManager(settingsManager)
    , m_currentMessages(new MessageListModel(this))
    , m_conversations(new ConversationListModel(this))
    , m_currentProviderId(m_settingsManager->defaultProvider())
{
    connect(m_settingsManager, &SettingsManager::apiKeysChanged, this, [this]() {
        emit registeredProvidersChanged();
    });
}

MessageListModel *ChatManager::currentMessages() const { return m_currentMessages; }
ConversationListModel *ChatManager::conversations() const { return m_conversations; }
QString ChatManager::currentConversationId() const { return m_currentConversationId; }
QString ChatManager::currentProviderId() const { return m_currentProviderId; }
QString ChatManager::currentModel() const { return m_currentModel; }
bool ChatManager::isStreaming() const { return m_isStreaming; }
bool ChatManager::hasActiveConversation() const { return !m_currentConversationId.isEmpty(); }

QStringList ChatManager::registeredProviders() const
{
    return m_settingsManager->providersWithKeys();
}

void ChatManager::sendMessage(const QString &text)
{
    if (text.trimmed().isEmpty() || m_isStreaming)
        return;

    ensureConversation();

    if (m_currentModel.isEmpty()) {
        m_currentModel = m_settingsManager->defaultModel(m_currentProviderId);
    }

    // Create user message
    MessageData userMsg;
    userMsg.conversationId = m_currentConversationId;
    userMsg.role = "user";
    userMsg.content = text.trimmed();
    userMsg.providerName = m_currentProviderId;
    userMsg.modelName = m_currentModel;
    userMsg.timestamp = QDateTime::currentDateTime();

    m_currentMessages->appendMessage(userMsg);
    persistMessage({{"role", userMsg.role}, {"content", userMsg.content}});

    // Build history for provider
    QJsonArray history;
    QVector<MessageData> allMsgs = m_currentMessages->allMessages();
    for (int i = 0; i < allMsgs.size() - 1; ++i) {
        const auto &msg = allMsgs[i];
        if (msg.role != "system" && !msg.isStreaming) {
            QJsonObject hMsg;
            hMsg["role"] = msg.role;
            hMsg["content"] = msg.content;
            history.append(hMsg);
        }
    }

    // Get provider
    AiProviderBase *provider = m_providerFactory->createProvider(m_currentProviderId);
    if (!provider) {
        emit errorOccurred("Provider Error",
                           QString("Unknown provider: %1").arg(m_currentProviderId));
        return;
    }

    if (!provider->isAvailable()) {
        emit errorOccurred("No API Key",
                           QString("Please configure an API key for %1 in Settings.")
                               .arg(provider->displayName()));
        return;
    }

    // Connect signals
    connect(provider, &AiProviderBase::responseChunkReceived,
            this, &ChatManager::onResponseChunk, Qt::UniqueConnection);
    connect(provider, &AiProviderBase::responseComplete,
            this, &ChatManager::onResponseComplete, Qt::UniqueConnection);
    connect(provider, &AiProviderBase::errorOccurred,
            this, &ChatManager::onProviderError, Qt::UniqueConnection);

    // Create placeholder assistant message for streaming
    MessageData assistantMsg;
    assistantMsg.conversationId = m_currentConversationId;
    assistantMsg.role = "assistant";
    assistantMsg.content = "";
    assistantMsg.providerName = m_currentProviderId;
    assistantMsg.modelName = m_currentModel;
    assistantMsg.timestamp = QDateTime::currentDateTime();
    assistantMsg.isStreaming = true;
    m_currentMessages->appendMessage(assistantMsg);

    m_isStreaming = true;
    emit isStreamingChanged();

    provider->sendMessage(text.trimmed(), history, m_currentModel, true);
}

void ChatManager::cancelCurrentRequest()
{
    AiProviderBase *provider = m_providerFactory->createProvider(m_currentProviderId);
    if (provider) {
        provider->cancelCurrentRequest();
    }
    m_isStreaming = false;
    emit isStreamingChanged();
}

void ChatManager::createNewConversation(const QString &providerId)
{
    QString pid = providerId.isEmpty() ? m_currentProviderId : providerId;
    QString model = m_settingsManager->defaultModel(pid);

    m_currentConversationId = m_databaseManager->createConversation("New Chat", pid, model);
    m_currentProviderId = pid;
    m_currentModel = model;
    m_currentMessages->clear();

    refreshConversations();
    emit currentConversationChanged();
    emit currentProviderIdChanged();
    emit currentModelChanged();
}

void ChatManager::switchConversation(const QString &conversationId)
{
    if (conversationId.isEmpty() || conversationId == m_currentConversationId)
        return;

    ConversationData conv = m_databaseManager->loadConversation(conversationId);
    if (conv.id.isEmpty())
        return;

    m_currentConversationId = conv.id;
    m_currentProviderId = conv.providerName;
    m_currentModel = conv.modelName;

    auto messages = m_databaseManager->loadMessages(conversationId);
    m_currentMessages->setMessages(messages);

    emit currentConversationChanged();
    emit currentProviderIdChanged();
    emit currentModelChanged();
}

void ChatManager::deleteConversation(const QString &conversationId)
{
    m_databaseManager->deleteConversation(conversationId);

    if (conversationId == m_currentConversationId) {
        m_currentConversationId.clear();
        m_currentMessages->clear();
        emit currentConversationChanged();
    }

    refreshConversations();
}

void ChatManager::renameConversation(const QString &conversationId, const QString &title)
{
    m_databaseManager->updateConversation(conversationId, title);
    refreshConversations();
}

void ChatManager::setCurrentProviderId(const QString &providerId)
{
    if (providerId == m_currentProviderId)
        return;

    m_currentProviderId = providerId;
    m_currentModel = m_settingsManager->defaultModel(providerId);
    m_settingsManager->setDefaultProvider(providerId);
    emit currentProviderIdChanged();
    emit currentModelChanged();
}

void ChatManager::setCurrentModel(const QString &modelName)
{
    if (modelName == m_currentModel)
        return;

    m_currentModel = modelName;
    m_settingsManager->setDefaultModel(m_currentProviderId, modelName);
    emit currentModelChanged();
}

QStringList ChatManager::availableModels(const QString &providerId) const
{
    AiProviderBase *provider = m_providerFactory->createProvider(providerId);
    if (provider)
        return provider->supportedModels();
    return {};
}

void ChatManager::loadLastSession()
{
    refreshConversations();
    auto allConvs = m_conversations->allConversations();
    if (!allConvs.isEmpty()) {
        switchConversation(allConvs.first().id);
    }
}

void ChatManager::onResponseChunk(const QString &chunk, const QString &conversationId)
{
    Q_UNUSED(conversationId)
    m_currentMessages->updateLastAssistantMessage(chunk);
}

void ChatManager::onResponseComplete(const QString &fullResponse, const QString &conversationId,
                                      const QVariantMap &usage)
{
    Q_UNUSED(conversationId)
    Q_UNUSED(usage)

    m_currentMessages->finalizeLastAssistantMessage(fullResponse, m_currentModel);
    m_isStreaming = false;
    emit isStreamingChanged();

    // Persist to database
    QJsonObject assistantMsg;
    assistantMsg["role"] = "assistant";
    assistantMsg["content"] = fullResponse;
    persistMessage(assistantMsg);

    // Update conversation metadata
    QString preview = fullResponse.left(100);
    m_databaseManager->updateConversation(m_currentConversationId, QString(), preview, 2);

    refreshConversations();
}

void ChatManager::onProviderError(const QString &errorMessage, int httpStatusCode)
{
    m_isStreaming = false;
    emit isStreamingChanged();

    // Remove placeholder streaming message
    auto allMsgs = m_currentMessages->allMessages();
    if (!allMsgs.isEmpty() && allMsgs.last().isStreaming) {
        // Simply mark as not streaming with error content
        m_currentMessages->finalizeLastAssistantMessage("Error: " + errorMessage, m_currentModel);
    }

    QString title = "API Error";
    if (httpStatusCode == 401 || httpStatusCode == 403) {
        title = "Invalid API Key";
    } else if (httpStatusCode == 429) {
        title = "Rate Limited";
    }

    emit errorOccurred(title, errorMessage);
}

void ChatManager::persistMessage(const QJsonObject &msg)
{
    MessageData data;
    data.conversationId = m_currentConversationId;
    data.role = msg["role"].toString();
    data.content = msg["content"].toString();
    data.providerName = m_currentProviderId;
    data.modelName = m_currentModel;
    data.timestamp = QDateTime::currentDateTime();
    m_databaseManager->insertMessage(data);
}

void ChatManager::refreshConversations()
{
    auto convs = m_databaseManager->loadAllConversations();
    m_conversations->setConversations(convs);
}

void ChatManager::ensureConversation()
{
    if (m_currentConversationId.isEmpty()) {
        createNewConversation(m_currentProviderId);
    }
}
