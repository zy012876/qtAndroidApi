#pragma once

#include "AiProviderBase.h"

class GeminiProvider : public AiProviderBase
{
    Q_OBJECT

public:
    using AiProviderBase::AiProviderBase;

    QString providerId() const override { return QStringLiteral("gemini"); }
    QString displayName() const override { return QStringLiteral("Google Gemini"); }
    QString defaultEndpoint() const override {
        return QStringLiteral("https://generativelanguage.googleapis.com/v1beta/models/");
    }
    QStringList supportedModels() const override {
        return {"gemini-2.5-flash", "gemini-2.5-pro", "gemini-1.5-pro", "gemini-1.5-flash"};
    }

protected:
    QNetworkRequest buildRequest(const QString &model, bool stream,
                                 const QVariantMap &extraParams) override;
    QByteArray buildRequestBody(const QString &userMessage, const QJsonArray &history,
                                const QString &model, bool stream) override;
    void processResponseChunk(const QByteArray &chunk) override;
    QPair<QString, QVariantMap> parseFinalResponse(const QByteArray &data) override;

private:
    QString m_partialLine;
    QString m_currentModel;
};
