pragma Singleton
import QtQuick

QtObject {
    // Colors
    property color backgroundColor: "#1a1a2e"
    property color surfaceColor: "#16213e"
    property color cardColor: "#0f3460"
    property color accentColor: "#e94560"
    property color primaryTextColor: "#ffffff"
    property color secondaryTextColor: "#a0a0b0"
    property color tertiaryTextColor: "#6c6c80"
    property color userBubbleColor: "#0f3460"
    property color assistantBubbleColor: "#16213e"
    property color chipColor: "#1a1a2e"
    property color dividerColor: "#2a2a4a"
    property color inputBarColor: "#16213e"

    // Layout
    property int pageMargin: 16
    property int bubbleRadius: 12
    property int chipRadius: 18

    // Typography
    property int fontSizeSmall: 11
    property int fontSizeBody: 14
    property int fontSizeTitle: 18
}
