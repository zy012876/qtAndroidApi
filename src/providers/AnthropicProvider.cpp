#include "AnthropicProvider.h"

QNetworkRequest AnthropicProvider::buildRequest(const QString &model, bool stream,
                                                  const QVariantMap &extraParams)
{
    Q_UNUSED(model)
    Q_UNUSED(extraParams)

    QNetworkRequest request{QUrl{defaultEndpoint()}};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("x-api-key", m_apiKey.toUtf8());
    request.setRawHeader("anthropic-version", "2023-06-01");
    if (stream) {
        request.setRawHeader("Accept", "text/event-stream");
    }
    return request;
}

QByteArray AnthropicProvider::buildRequestBody(const QString &userMessage,
                                                 const QJsonArray &history,
                                                 const QString &model,
                                                 bool stream)
{
    QJsonObject body;
    body["model"] = model;
    body["stream"] = stream;
    body["max_tokens"] = 4096;

    QJsonArray messages;

    // Extract system messages from history
    for (const auto &item : history) {
        QJsonObject msg = item.toObject();
        if (msg["role"].toString() == "system") {
            body["system"] = msg["content"].toString();
        } else {
            messages.append(msg);
        }
    }

    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = userMessage;
    messages.append(userMsg);

    body["messages"] = messages;

    return QJsonDocument(body).toJson(QJsonDocument::Compact);
}

void AnthropicProvider::processResponseChunk(const QByteArray &chunk, QString & /*partialLine*/)
{
    m_partialLine += QString::fromUtf8(chunk);

    QStringList lines = m_partialLine.split('\n');
    m_partialLine = lines.takeLast();

    for (const QString &line : lines) {
        if (line.startsWith("event: ")) {
            m_eventType = line.mid(7).trimmed();
            continue;
        }

        if (!line.startsWith("data: "))
            continue;

        QString data = line.mid(6);
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        if (doc.isNull())
            continue;

        QJsonObject obj = doc.object();

        if (m_eventType == "content_block_delta") {
            QString text = obj["delta"].toObject()["text"].toString();
            if (!text.isEmpty()) {
                m_accumulatedResponse += text;
                emit responseChunkReceived(text, m_currentConversationId);
            }
        } else if (m_eventType == "message_delta") {
            // Usage info comes here
        } else if (m_eventType == "message_start") {
            emit streamingStarted(m_currentConversationId);
        }
    }
}

QPair<QString, QVariantMap> AnthropicProvider::parseFinalResponse(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    QString content;
    QJsonArray contentBlocks = obj["content"].toArray();
    for (const auto &block : contentBlocks) {
        QJsonObject blockObj = block.toObject();
        if (blockObj["type"].toString() == "text") {
            content += blockObj["text"].toString();
        }
    }

    QVariantMap metadata;
    QJsonObject usage = obj["usage"].toObject();
    if (!usage.isEmpty()) {
        metadata["inputTokens"] = usage["input_tokens"].toInt();
        metadata["outputTokens"] = usage["output_tokens"].toInt();
    }

    return {content.isEmpty() ? m_accumulatedResponse : content, metadata};
}
