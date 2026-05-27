#include "AndroidKeyStore.h"

#ifdef Q_OS_ANDROID

#include <QDebug>
#include <QJniEnvironment>

AndroidKeyStore::AndroidKeyStore(QObject *parent)
    : QObject(parent)
{
    QJniEnvironment env;
    jclass keyStoreClass = env->FindClass("java/security/KeyStore");
    if (keyStoreClass) {
        jmethodID getInstance = env->GetStaticMethodID(
            keyStoreClass, "getInstance",
            "(Ljava/lang/String;)Ljava/security/KeyStore;");
        jstring provider = env->NewStringUTF("AndroidKeyStore");
        jobject ks = env->CallStaticObjectMethod(keyStoreClass, getInstance, provider);
        if (ks) {
            jmethodID load = env->GetMethodID(keyStoreClass, "load",
                                               "(Ljava/security/KeyStore$LoadStoreParameter;)V");
            env->CallVoidMethod(ks, load, nullptr);
            m_keyStore = QJniObject(ks);
        }
        env->DeleteLocalRef(provider);
    }
}

bool AndroidKeyStore::isAvailable() const
{
    return m_keyStore.isValid();
}

bool AndroidKeyStore::storeKey(const QString &alias, const QString &key)
{
    if (!isAvailable())
        return false;

    QJniObject javaAlias = QJniObject::fromString(alias);
    QJniObject javaKey = QJniObject::fromString(key);

    QJniObject result = QJniObject::callStaticObjectMethod(
        "org/qtproject/aichat/KeyStoreHelper",
        "encryptAndStore",
        "(Ljava/lang/String;Ljava/lang/String;)Z",
        javaAlias.object<jstring>(),
        javaKey.object<jstring>());

    return result.isValid();
}

QString AndroidKeyStore::loadKey(const QString &alias) const
{
    if (!isAvailable())
        return {};

    QJniObject javaAlias = QJniObject::fromString(alias);

    QJniObject result = QJniObject::callStaticObjectMethod(
        "org/qtproject/aichat/KeyStoreHelper",
        "decryptAndLoad",
        "(Ljava/lang/String;)Ljava/lang/String;",
        javaAlias.object<jstring>());

    return result.toString();
}

bool AndroidKeyStore::deleteKey(const QString &alias)
{
    if (!isAvailable())
        return false;

    QJniObject javaAlias = QJniObject::fromString(alias);

    QJniObject result = QJniObject::callStaticObjectMethod(
        "org/qtproject/aichat/KeyStoreHelper",
        "deleteEntry",
        "(Ljava/lang/String;)Z",
        javaAlias.object<jstring>());

    return result.isValid();
}

#endif
