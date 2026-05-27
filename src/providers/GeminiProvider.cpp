#include "GeminiProvider.h"

QNetworkRequest GeminiProvider::buildRequest(const QString &model, bool stream,
                                              const QVariantMap &extraParams)
{
    Q_UNUSED(extraParams)

    m_currentModel = model;

    QString endpoint = defaultEndpoint() + model;
    if (stream) {
        endpoint += ":streamGenerateContent?alt=sse&key=" + m_apiKey;
    } else {
        endpoint += ":generateContent?key=" + m_apiKey;
    }

    QNetworkRequest request(QUrl(endpoint));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (stream) {
        request.setRawHeader("Accept", "text/event-stream");
    }
    return request;
}

QByteArray GeminiProvider::buildRequestBody(const QString &userMessage,
                                              const QJsonArray &history,
                                              const QString &model,
                                              bool stream)
{
    Q_UNUSED(model)

    QJsonObject body;

    QJsonArray contents;

    // Build from history
    for (const auto &item : history) {
        QJsonObject msg = item.toObject();
        QJsonObject content;
        content["role"] = msg["role"].toString() == "assistant" ? "model" : "user";

        QJsonArray parts;
        QJsonObject textPart;
        textPart["text"] = msg["content"].toString();
        parts.append(textPart);
        content["parts"] = parts;

        contents.append(content);
    }

    // Add current message
    QJsonObject userContent;
    userContent["role"] = "user";
    QJsonArray userParts;
    QJsonObject userTextPart;
    userTextPart["text"] = userMessage;
    userParts.append(userTextPart);
    userContent["parts"] = userParts;
    contents.append(userContent);

    body["contents"] = contents;

    // Generation config
    QJsonObject generationConfig;
    generationConfig["temperature"] = 0.7;
    generationConfig["maxOutputTokens"] = 4096;
    body["generationConfig"] = generationConfig;

    QJsonDocument doc(body);
    return doc.toJson(QJsonDocument::Compact);
}

void GeminiProvider::processResponseChunk(const QByteArray &chunk, QString & /*partialLine*/)
{
    m_partialLine += QString::fromUtf8(chunk);

    QStringList lines = m_partialLine.split('\n');
    m_partialLine = lines.takeLast();

    for (const QString &line : lines) {
        if (!line.startsWith("data: "))
            continue;

        QString data = line.mid(6).trimmed();
        if (data.isEmpty())
            continue;

        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        if (doc.isNull())
            continue;

        QJsonObject obj = doc.object();
        QJsonArray candidates = obj["candidates"].toArray();
        if (candidates.isEmpty())
            continue;

        QJsonObject content = candidates[0]["content"].toObject();
        QJsonArray parts = content["parts"].toArray();
        if (parts.isEmpty())
            continue;

        QString text = parts[0]["text"].toString();
        if (!text.isEmpty()) {
            m_accumulatedResponse += text;
            emit responseChunkReceived(text, m_currentConversationId);
        }
    }
}

QPair<QString, QVariantMap> GeminiProvider::parseFinalResponse(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    QString content;
    QJsonArray candidates = obj["candidates"].toArray();
    if (!candidates.isEmpty()) {
        QJsonObject contentObj = candidates[0]["content"].toObject();
        QJsonArray parts = contentObj["parts"].toArray();
        for (const auto &part : parts) {
            content += part.toObject()["text"].toString();
        }
    }

    QVariantMap metadata;
    QJsonObject usageMeta = obj["usageMetadata"].toObject();
    if (!usageMeta.isEmpty()) {
        metadata["promptTokens"] = usageMeta["promptTokenCount"].toInt();
        metadata["completionTokens"] = usageMeta["candidatesTokenCount"].toInt();
        metadata["totalTokens"] = usageMeta["totalTokenCount"].toInt();
    }

    return {content.isEmpty() ? m_accumulatedResponse : content, metadata};
}
