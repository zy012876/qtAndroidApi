#pragma once

#include <QObject>
#include <QString>

#ifdef Q_OS_ANDROID

#include <QJniObject>

class AndroidKeyStore : public QObject
{
    Q_OBJECT

public:
    explicit AndroidKeyStore(QObject *parent = nullptr);

    bool isAvailable() const;
    bool storeKey(const QString &alias, const QString &key);
    QString loadKey(const QString &alias) const;
    bool deleteKey(const QString &alias);

private:
    QJniObject m_keyStore;
};

#else

class AndroidKeyStore : public QObject
{
    Q_OBJECT

public:
    explicit AndroidKeyStore(QObject *parent = nullptr) : QObject(parent) {}
    bool isAvailable() const { return false; }
    bool storeKey(const QString &, const QString &) { return false; }
    QString loadKey(const QString &) const { return {}; }
    bool deleteKey(const QString &) { return false; }
};

#endif
