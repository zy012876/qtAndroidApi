#include "ProviderFactory.h"
#include "OpenAiProvider.h"
#include "AnthropicProvider.h"
#include "GeminiProvider.h"
#include "managers/SettingsManager.h"

ProviderFactory::ProviderFactory(QNetworkAccessManager *networkManager,
                                   SettingsManager *settings,
                                   QObject *parent)
    : QObject(parent)
    , m_networkManager(networkManager)
    , m_settings(settings)
{
}

AiProviderBase *ProviderFactory::createProvider(const QString &providerId)
{
    if (m_providers.contains(providerId))
        return m_providers[providerId];

    AiProviderBase *provider = nullptr;

    if (providerId == "openai") {
        provider = new OpenAiProvider(m_networkManager, this);
    } else if (providerId == "anthropic") {
        provider = new AnthropicProvider(m_networkManager, this);
    } else if (providerId == "gemini") {
        provider = new GeminiProvider(m_networkManager, this);
    }

    if (provider) {
        QString key = m_settings->loadApiKey(providerId);
        if (!key.isEmpty())
            provider->setApiKey(key);
        m_providers[providerId] = provider;
    }

    return provider;
}

QStringList ProviderFactory::availableProviderIds() const
{
    return {"openai", "anthropic", "gemini"};
}

QStringList ProviderFactory::registeredProviderIds() const
{
    return m_settings->providersWithKeys();
}

AiProviderBase *ProviderFactory::currentProvider() const
{
    return nullptr;
}
