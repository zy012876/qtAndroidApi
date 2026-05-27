#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QFileInfo>
#include <QCoreApplication>

#include "managers/ChatManager.h"
#include "managers/SettingsManager.h"
#include "managers/DatabaseManager.h"
#include "providers/ProviderFactory.h"
#include "ThemeProvider.h"

static QFile g_logFile;

static void logToFile(const QString &msg)
{
    if (!g_logFile.isOpen()) return;
    QTextStream ts(&g_logFile);
    ts << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << " " << msg << "\n";
    ts.flush();
}

static void messageHandler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
    Q_UNUSED(ctx)
    QString prefix;
    switch (type) {
    case QtDebugMsg:    prefix = "DEBUG"; break;
    case QtInfoMsg:     prefix = "INFO"; break;
    case QtWarningMsg:  prefix = "WARN"; break;
    case QtCriticalMsg: prefix = "CRITICAL"; break;
    case QtFatalMsg:    prefix = "FATAL"; break;
    }
    logToFile(QString("%1: %2").arg(prefix, msg));
}

int main(int argc, char *argv[])
{
    // Init log file on desktop for easy access
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)
                      + "/qtAndroidApi.log";
    g_logFile.setFileName(logPath);
    g_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
    qInstallMessageHandler(messageHandler);

    logToFile("App starting...");

    QGuiApplication app(argc, argv);
    app.setOrganizationName("AIChatApp");
    app.setOrganizationDomain("aichat.local");
    app.setApplicationName("AI Chat");

    // 1. Database setup
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    bool dirOk = QDir().mkpath(dataDir);
    logToFile(QString("Data dir: %1, created: %2").arg(dataDir).arg(dirOk));

    QString dbPath = dataDir + "/conversations.db";
    logToFile(QString("DB path: %1").arg(dbPath));

    auto *databaseManager = new DatabaseManager(dbPath, &app);
    if (!databaseManager->initialize()) {
        logToFile("FATAL: Database init FAILED");
        return 1;
    }
    logToFile("Database init OK");

    // 2. Managers
    auto *settingsManager = new SettingsManager(&app);
    auto *networkManager = new QNetworkAccessManager(&app);
    networkManager->setTransferTimeout(120000);

    auto *providerFactory = new ProviderFactory(networkManager, settingsManager, &app);
    auto *chatManager = new ChatManager(providerFactory, databaseManager,
                                         settingsManager, &app);
    logToFile("Managers created OK");

    // 3. QML Engine
    QQmlApplicationEngine engine;

    QQmlContext *context = engine.rootContext();
    context->setContextProperty("chatManager", chatManager);
    context->setContextProperty("settingsManager", settingsManager);
    context->setContextProperty("providerFactory", providerFactory);
    context->setContextProperty("Theme", new ThemeProvider(&app));

#if defined(Q_OS_ANDROID)
    QQuickStyle::setStyle("Material");
#else
    QQuickStyle::setStyle("Fusion");
#endif

    const QUrl url(QStringLiteral("qrc:/AIChat/qml/main.qml"));
    logToFile(QString("Loading QML from: %1").arg(url.toString()));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, [&]() {
            logToFile("QML objectCreationFailed signal fired");
            QCoreApplication::exit(-2);
        },
        Qt::QueuedConnection);

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [&](QObject *obj, const QUrl &objUrl) {
            if (!obj) {
                logToFile(QString("QML failed: %1").arg(objUrl.toString()));
                QCoreApplication::exit(-2);
            } else {
                logToFile("QML object created successfully");
            }
        },
        Qt::QueuedConnection);

    engine.load(url);
    logToFile(QString("Root objects count: %1").arg(engine.rootObjects().size()));

    if (engine.rootObjects().isEmpty()) {
        logToFile("No root objects — waiting for async load...");
    }

    logToFile("Entering event loop");
    int ret = app.exec();
    logToFile(QString("App exited with code: %1").arg(ret));
    return ret;
}
