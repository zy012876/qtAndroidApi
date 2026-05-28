#include "OpenAiProvider.h"

QNetworkRequest OpenAiProvider::buildRequest(const QString &model, bool stream,
                                              const QVariantMap &extraParams)
{
    Q_UNUSED(model)
    Q_UNUSED(extraParams)

    QNetworkRequest request{QUrl{defaultEndpoint()}};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());
    request.setRawHeader("Accept", stream ? "text/event-stream" : "application/json");
    return request;
}

QByteArray OpenAiProvider::buildRequestBody(const QString &userMessage,
                                             const QJsonArray &history,
                                             const QString &model,
                                             bool stream)
{
    QJsonObject body;
    body["model"] = model;
    body["stream"] = stream;

    QJsonArray messages = history;
    QJsonObject userMsg;
    userMsg["role"] = "user";
    userMsg["content"] = userMessage;
    messages.append(userMsg);

    body["messages"] = messages;

    return QJsonDocument(body).toJson(QJsonDocument::Compact);
}

void OpenAiProvider::processResponseChunk(const QByteArray &chunk)
{
    m_partialLine += QString::fromUtf8(chunk);

    QStringList lines = m_partialLine.split('\n');
    m_partialLine = lines.takeLast();

    for (const QString &line : lines) {
        if (!line.startsWith("data: "))
            continue;

        QString data = line.mid(6).trimmed();
        if (data.isEmpty() || data == "[DONE]")
            continue;

        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        if (doc.isNull())
            continue;

        QJsonObject obj = doc.object();
        QJsonArray choices = obj["choices"].toArray();
        if (choices.isEmpty())
            continue;

        QJsonValue first = choices[0];
        QJsonObject delta = first["delta"].toObject();
        QString content = delta["content"].toString();

        if (!content.isEmpty()) {
            m_accumulatedResponse += content;
            emit responseChunkReceived(content, m_currentConversationId);
        }
    }
}

QPair<QString, QVariantMap> OpenAiProvider::parseFinalResponse(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    QJsonArray choices = obj["choices"].toArray();
    QString content;
    if (!choices.isEmpty()) {
        QJsonValue firstMsg = choices[0];
        content = firstMsg["message"].toObject()["content"].toString();
    }

    QVariantMap metadata;
    QJsonObject usage = obj["usage"].toObject();
    if (!usage.isEmpty()) {
        metadata["promptTokens"] = usage["prompt_tokens"].toInt();
        metadata["completionTokens"] = usage["completion_tokens"].toInt();
        metadata["totalTokens"] = usage["total_tokens"].toInt();
    }

    return {content.isEmpty() ? m_accumulatedResponse : content, metadata};
}
