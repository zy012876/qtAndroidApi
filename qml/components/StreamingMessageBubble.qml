import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: bubbleRoot
    property string msgRole: "assistant"
    property string msgContent: ""
    property string msgProviderName: ""
    property string msgModelName: ""
    property string msgTimestampFormatted: ""
    property bool msgIsStreaming: true

    width: parent ? parent.width : 340
    height: bubbleContainer.height + 16

    Rectangle {
        id: bubbleContainer
        width: parent.width - 64
        height: contentCol.childrenRect.height + 20
        radius: Theme.bubbleRadius
        color: Theme.assistantBubbleColor
        anchors {
            left: parent.left
            leftMargin: 16
        }

        Column {
            id: contentCol
            anchors.fill: parent
            anchors.margins: 12
            spacing: 4
            width: parent.width

            Row {
                spacing: 4
                Image {
                    source: "qrc:/icons/" + msgProviderName + ".svg"
                    width: 14; height: 14
                }
                Label {
                    text: msgProviderName === "openai" ? "OpenAI" :
                          (msgProviderName === "anthropic" ? "Claude" :
                           msgProviderName === "gemini" ? "Gemini" :
                           msgProviderName === "deepseek" ? "DeepSeek" : msgProviderName)
                    color: Theme.secondaryTextColor
                    font.pixelSize: Theme.fontSizeSmall
                }
            }

            Label {
                id: contentLabel
                text: msgContent + "▐"
                color: Theme.primaryTextColor
                font.pixelSize: Theme.fontSizeBody
                wrapMode: Text.Wrap
                textFormat: Text.PlainText
                width: parent.width
            }
        }

        // Blinking cursor overlay
        Rectangle {
            id: cursor
            visible: msgIsStreaming
            anchors.bottom: contentLabel.bottom
            anchors.bottomMargin: 2
            x: contentLabel.contentWidth + contentLabel.x + 2
            width: msgContent.length === 0 ? parent.width - 24 : 2
            height: msgContent.length === 0 ? parent.height - 24 : contentLabel.lineHeight

            color: Theme.accentColor
            opacity: 0.7

            SequentialAnimation on opacity {
                running: msgIsStreaming
                loops: Animation.Infinite
                NumberAnimation { from: 1; to: 0; duration: 500 }
                NumberAnimation { from: 0; to: 1; duration: 500 }
            }
        }
    }
}
