#pragma once

#include "AiProviderBase.h"

class OpenAiProvider : public AiProviderBase
{
    Q_OBJECT

public:
    using AiProviderBase::AiProviderBase;

    QString providerId() const override { return QStringLiteral("openai"); }
    QString displayName() const override { return QStringLiteral("OpenAI"); }
    QString defaultEndpoint() const override {
        return QStringLiteral("https://api.openai.com/v1/chat/completions");
    }
    QStringList supportedModels() const override {
        return {"gpt-4o", "gpt-4o-mini", "gpt-4-turbo", "gpt-3.5-turbo"};
    }
    bool supportsSystemPrompt() const override { return true; }

protected:
    QNetworkRequest buildRequest(const QString &model, bool stream,
                                 const QVariantMap &extraParams) override;
    QByteArray buildRequestBody(const QString &userMessage, const QJsonArray &history,
                                const QString &model, bool stream) override;
    void processResponseChunk(const QByteArray &chunk, QString &partialLine) override;
    QPair<QString, QVariantMap> parseFinalResponse(const QByteArray &data) override;

private:
    QString m_partialLine;
};
