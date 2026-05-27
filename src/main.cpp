#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QDebug>

#include "managers/ChatManager.h"
#include "managers/SettingsManager.h"
#include "managers/DatabaseManager.h"
#include "providers/ProviderFactory.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("AIChatApp");
    app.setOrganizationDomain("aichat.local");
    app.setApplicationName("AI Chat");

    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                     + "/conversations.db";
    qDebug() << "Database path:" << dbPath;

    auto *databaseManager = new DatabaseManager(dbPath, &app);
    if (!databaseManager->initialize()) {
        qCritical() << "Failed to initialize database at" << dbPath;
        return 1;
    }

    auto *settingsManager = new SettingsManager(&app);
    auto *networkManager = new QNetworkAccessManager(&app);
    networkManager->setTransferTimeout(120000);

    auto *providerFactory = new ProviderFactory(networkManager, settingsManager, &app);
    auto *chatManager = new ChatManager(providerFactory, databaseManager,
                                         settingsManager, &app);

    QQmlApplicationEngine engine;

    QQmlContext *context = engine.rootContext();
    context->setContextProperty("chatManager", chatManager);
    context->setContextProperty("settingsManager", settingsManager);
    context->setContextProperty("providerFactory", providerFactory);

#if defined(Q_OS_ANDROID)
    QQuickStyle::setStyle("Material");
#else
    QQuickStyle::setStyle("Fusion");
#endif

    const QUrl url(QStringLiteral("qrc:/AIChat/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
