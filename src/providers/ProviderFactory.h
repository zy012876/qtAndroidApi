#pragma once

#include <QObject>
#include <QHash>
#include <QNetworkAccessManager>

class AiProviderBase;
class SettingsManager;

class ProviderFactory : public QObject
{
    Q_OBJECT

public:
    explicit ProviderFactory(QNetworkAccessManager *networkManager,
                             SettingsManager *settings,
                             QObject *parent = nullptr);

    Q_INVOKABLE AiProviderBase *createProvider(const QString &providerId);
    Q_INVOKABLE QStringList availableProviderIds() const;
    Q_INVOKABLE QStringList registeredProviderIds() const;
    Q_INVOKABLE AiProviderBase *currentProvider() const;

private:
    QHash<QString, AiProviderBase *> m_providers;
    QNetworkAccessManager *m_networkManager;
    SettingsManager *m_settings;
};
