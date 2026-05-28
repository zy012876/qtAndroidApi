#include "AiProviderBase.h"

AiProviderBase::AiProviderBase(QNetworkAccessManager *networkManager, QObject *parent)
    : QObject(parent)
    , m_networkManager(networkManager)
{
}

void AiProviderBase::setApiKey(const QString &key)
{
    m_apiKey = key;
    emit availabilityChanged(isAvailable());
}

void AiProviderBase::setConversationId(const QString &id)
{
    m_currentConversationId = id;
}

QString AiProviderBase::apiKey() const
{
    return m_apiKey;
}

bool AiProviderBase::isAvailable() const
{
    return !m_apiKey.isEmpty();
}

void AiProviderBase::sendMessage(const QString &userMessage,
                                  const QJsonArray &messageHistory,
                                  const QString &model,
                                  bool stream,
                                  const QVariantMap &extraParams)
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }

    m_accumulatedResponse.clear();

    QNetworkRequest request = buildRequest(model, stream, extraParams);
    QByteArray body = buildRequestBody(userMessage, messageHistory, model, stream);

    m_currentReply = m_networkManager->post(request, body);

    connect(m_currentReply, &QNetworkReply::readyRead, this, [this]() {
        QByteArray chunk = m_currentReply->readAll();
        processResponseChunk(chunk);
    });

    connect(m_currentReply, &QNetworkReply::finished, this, [this]() {
        if (!m_currentReply)
            return;

        if (m_currentReply->error() != QNetworkReply::NoError) {
            int statusCode = m_currentReply->attribute(
                QNetworkRequest::HttpStatusCodeAttribute).toInt();
            emit errorOccurred(m_currentReply->errorString(), statusCode,
                               m_currentConversationId);
        } else {
            QByteArray data = m_currentReply->readAll();
            if (!data.isEmpty()) {
                auto [response, metadata] = parseFinalResponse(data);
                m_accumulatedResponse = response;
                emit responseComplete(m_accumulatedResponse, m_currentConversationId, metadata);
            } else {
                emit responseComplete(m_accumulatedResponse, m_currentConversationId, {});
            }
        }

        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        emit streamingFinished(m_currentConversationId);
    });
}

void AiProviderBase::cancelCurrentRequest()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}
