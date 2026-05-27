#pragma once

#include <QObject>
#include <QSettings>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);

    Q_INVOKABLE void saveApiKey(const QString &providerId, const QString &apiKey);
    Q_INVOKABLE QString loadApiKey(const QString &providerId) const;
    Q_INVOKABLE void deleteApiKey(const QString &providerId);
    Q_INVOKABLE bool hasApiKey(const QString &providerId) const;
    Q_INVOKABLE QStringList providersWithKeys() const;

    Q_INVOKABLE QString defaultProvider() const;
    Q_INVOKABLE void setDefaultProvider(const QString &providerId);
    Q_INVOKABLE QString defaultModel(const QString &providerId) const;
    Q_INVOKABLE void setDefaultModel(const QString &providerId, const QString &model);
    Q_INVOKABLE bool darkMode() const;
    Q_INVOKABLE void setDarkMode(bool enabled);

signals:
    void apiKeysChanged();
    void settingsChanged();

private:
    QString obfuscateKey(const QString &key) const;
    QString deobfuscateKey(const QString &obfuscated) const;

    QSettings m_settings;
    static constexpr const char *KEY_PREFIX = "apikey/";
    static constexpr quint8 XOR_CONSTANT = 0x5A;
};
