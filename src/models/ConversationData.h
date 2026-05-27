#pragma once

#include <QString>
#include <QDateTime>

struct ConversationData {
    QString id;
    QString title;
    QString providerName;
    QString modelName;
    QDateTime createdAt;
    QDateTime updatedAt;
    int messageCount = 0;
    QString preview;
};
