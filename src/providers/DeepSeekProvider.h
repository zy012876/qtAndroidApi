#pragma once

#include "AiProviderBase.h"

class DeepSeekProvider : public AiProviderBase
{
    Q_OBJECT

public:
    using AiProviderBase::AiProviderBase;

    QString providerId() const override { return QStringLiteral("deepseek"); }
    QString displayName() const override { return QStringLiteral("DeepSeek"); }
    QString defaultEndpoint() const override {
        return QStringLiteral("https://api.deepseek.com/chat/completions");
    }
    QStringList supportedModels() const override {
        return {"deepseek-v4-pro", "deepseek-chat", "deepseek-reasoner"};
    }
    bool supportsStreaming() const override { return true; }
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
