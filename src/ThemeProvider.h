#pragma once

#include <QObject>
#include <QColor>

class ThemeProvider : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QColor backgroundColor READ backgroundColor CONSTANT)
    Q_PROPERTY(QColor surfaceColor READ surfaceColor CONSTANT)
    Q_PROPERTY(QColor cardColor READ cardColor CONSTANT)
    Q_PROPERTY(QColor accentColor READ accentColor CONSTANT)
    Q_PROPERTY(QColor primaryTextColor READ primaryTextColor CONSTANT)
    Q_PROPERTY(QColor secondaryTextColor READ secondaryTextColor CONSTANT)
    Q_PROPERTY(QColor tertiaryTextColor READ tertiaryTextColor CONSTANT)
    Q_PROPERTY(QColor userBubbleColor READ userBubbleColor CONSTANT)
    Q_PROPERTY(QColor assistantBubbleColor READ assistantBubbleColor CONSTANT)
    Q_PROPERTY(QColor chipColor READ chipColor CONSTANT)
    Q_PROPERTY(QColor dividerColor READ dividerColor CONSTANT)
    Q_PROPERTY(QColor inputBarColor READ inputBarColor CONSTANT)
    Q_PROPERTY(int pageMargin READ pageMargin CONSTANT)
    Q_PROPERTY(int bubbleRadius READ bubbleRadius CONSTANT)
    Q_PROPERTY(int chipRadius READ chipRadius CONSTANT)
    Q_PROPERTY(int fontSizeSmall READ fontSizeSmall CONSTANT)
    Q_PROPERTY(int fontSizeBody READ fontSizeBody CONSTANT)
    Q_PROPERTY(int fontSizeTitle READ fontSizeTitle CONSTANT)

public:
    explicit ThemeProvider(QObject *parent = nullptr);

    QColor backgroundColor() const { return QColor("#1a1a2e"); }
    QColor surfaceColor() const { return QColor("#16213e"); }
    QColor cardColor() const { return QColor("#0f3460"); }
    QColor accentColor() const { return QColor("#e94560"); }
    QColor primaryTextColor() const { return QColor("#ffffff"); }
    QColor secondaryTextColor() const { return QColor("#a0a0b0"); }
    QColor tertiaryTextColor() const { return QColor("#6c6c80"); }
    QColor userBubbleColor() const { return QColor("#0f3460"); }
    QColor assistantBubbleColor() const { return QColor("#16213e"); }
    QColor chipColor() const { return QColor("#1a1a2e"); }
    QColor dividerColor() const { return QColor("#2a2a4a"); }
    QColor inputBarColor() const { return QColor("#16213e"); }
    int pageMargin() const { return 16; }
    int bubbleRadius() const { return 12; }
    int chipRadius() const { return 18; }
    int fontSizeSmall() const { return 11; }
    int fontSizeBody() const { return 14; }
    int fontSizeTitle() const { return 18; }
};
