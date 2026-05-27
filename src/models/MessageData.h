#pragma once

#include <QString>
#include <QDateTime>

struct MessageData {
    qint64 id = -1;
    QString conversationId;
    QString role;          // "user", "assistant", "system"
    QString content;
    QString providerName;  // "openai", "anthropic", "gemini"
    QString modelName;
    QDateTime timestamp;
    bool isStreaming = false;
};
