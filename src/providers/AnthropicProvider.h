#pragma once

#include "AiProviderBase.h"

class AnthropicProvider : public AiProviderBase
{
    Q_OBJECT

public:
    using AiProviderBase::AiProviderBase;

    QString providerId() const override { return QStringLiteral("anthropic"); }
    QString displayName() const override { return QStringLiteral("Anthropic Claude"); }
    QString defaultEndpoint() const override {
        return QStringLiteral("https://api.anthropic.com/v1/messages");
    }
    QStringList supportedModels() const override {
        return {"claude-sonnet-4-20250514", "claude-3-5-sonnet-20241022",
                "claude-3-opus-20240229", "claude-3-5-haiku-20241022"};
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
    QString m_eventType;
};
