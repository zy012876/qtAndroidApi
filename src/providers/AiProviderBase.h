#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>

class AiProviderBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString providerId READ providerId CONSTANT)
    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(bool supportsStreaming READ supportsStreaming CONSTANT)
    Q_PROPERTY(bool isAvailable READ isAvailable NOTIFY availabilityChanged)
    Q_PROPERTY(QStringList supportedModels READ supportedModels CONSTANT)

public:
    explicit AiProviderBase(QNetworkAccessManager *networkManager,
                            QObject *parent = nullptr);

    virtual QString providerId() const = 0;
    virtual QString displayName() const = 0;
    virtual QString defaultEndpoint() const = 0;
    virtual QStringList supportedModels() const { return {}; }
    virtual bool supportsStreaming() const { return true; }

    void setApiKey(const QString &key);
    void setConversationId(const QString &id);
    QString apiKey() const;
    bool isAvailable() const;

    virtual void sendMessage(const QString &userMessage,
                             const QJsonArray &messageHistory,
                             const QString &model,
                             bool stream,
                             const QVariantMap &extraParams = {});
    void cancelCurrentRequest();

signals:
    void responseChunkReceived(const QString &chunk, const QString &conversationId);
    void responseComplete(const QString &fullResponse, const QString &conversationId,
                          const QVariantMap &usageMetadata);
    void errorOccurred(const QString &errorMessage, int httpStatusCode,
                       const QString &conversationId);
    void streamingStarted(const QString &conversationId);
    void streamingFinished(const QString &conversationId);
    void availabilityChanged(bool available);

protected:
    virtual QNetworkRequest buildRequest(const QString &model, bool stream,
                                         const QVariantMap &extraParams) = 0;
    virtual QByteArray buildRequestBody(const QString &userMessage,
                                        const QJsonArray &messageHistory,
                                        const QString &model,
                                        bool stream) = 0;
    virtual void processResponseChunk(const QByteArray &chunk) = 0;
    virtual QPair<QString, QVariantMap> parseFinalResponse(const QByteArray &data) = 0;

    QNetworkAccessManager *m_networkManager;
    QString m_apiKey;
    QNetworkReply *m_currentReply = nullptr;
    QString m_currentConversationId;
    QString m_accumulatedResponse;
};
