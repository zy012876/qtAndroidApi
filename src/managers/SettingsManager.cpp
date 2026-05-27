#include "SettingsManager.h"

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
{
}

void SettingsManager::saveApiKey(const QString &providerId, const QString &apiKey)
{
    m_settings.setValue(KEY_PREFIX + providerId, obfuscateKey(apiKey));
    emit apiKeysChanged();
}

QString SettingsManager::loadApiKey(const QString &providerId) const
{
    QString val = m_settings.value(KEY_PREFIX + providerId).toString();
    if (val.isEmpty()) return {};
    return deobfuscateKey(val);
}

void SettingsManager::deleteApiKey(const QString &providerId)
{
    m_settings.remove(KEY_PREFIX + providerId);
    emit apiKeysChanged();
}

bool SettingsManager::hasApiKey(const QString &providerId) const
{
    return m_settings.contains(KEY_PREFIX + providerId);
}

QStringList SettingsManager::providersWithKeys() const
{
    QStringList result;
    m_settings.beginGroup(QString(KEY_PREFIX).chopped(1));
    result = m_settings.childKeys();
    m_settings.endGroup();
    return result;
}

QString SettingsManager::defaultProvider() const
{
    return m_settings.value("defaultProvider", "openai").toString();
}

void SettingsManager::setDefaultProvider(const QString &providerId)
{
    m_settings.setValue("defaultProvider", providerId);
    emit settingsChanged();
}

QString SettingsManager::defaultModel(const QString &providerId) const
{
    QString key = "defaultModel/" + providerId;
    if (providerId == "openai")
        return m_settings.value(key, "gpt-4o").toString();
    if (providerId == "anthropic")
        return m_settings.value(key, "claude-sonnet-4-20250514").toString();
    if (providerId == "gemini")
        return m_settings.value(key, "gemini-2.5-flash").toString();
    return m_settings.value(key).toString();
}

void SettingsManager::setDefaultModel(const QString &providerId, const QString &model)
{
    m_settings.setValue("defaultModel/" + providerId, model);
    emit settingsChanged();
}

bool SettingsManager::darkMode() const
{
    return m_settings.value("darkMode", true).toBool();
}

void SettingsManager::setDarkMode(bool enabled)
{
    m_settings.setValue("darkMode", enabled);
    emit settingsChanged();
}

QString SettingsManager::obfuscateKey(const QString &key) const
{
    QByteArray data = key.toUtf8();
    for (int i = 0; i < data.size(); ++i)
        data[i] = data[i] ^ XOR_CONSTANT;
    return QString::fromLatin1(data.toBase64());
}

QString SettingsManager::deobfuscateKey(const QString &obfuscated) const
{
    QByteArray data = QByteArray::fromBase64(obfuscated.toLatin1());
    for (int i = 0; i < data.size(); ++i)
        data[i] = data[i] ^ XOR_CONSTANT;
    return QString::fromUtf8(data);
}
