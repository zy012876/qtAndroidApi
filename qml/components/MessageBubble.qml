import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: bubbleRoot
    property string msgRole: "user"
    property string msgContent: ""
    property string msgProviderName: ""
    property string msgModelName: ""
    property string msgTimestampFormatted: ""
    property bool msgIsStreaming: false

    width: parent ? parent.width : 340
    height: bubbleContainer.height + 16

    property bool isUser: msgRole === "user"

    Rectangle {
        id: bubbleContainer
        width: Math.min(implicitWidth, parent.width - 64)
        height: contentCol.implicitHeight + 20
        radius: Theme.bubbleRadius
        color: isUser ? Theme.userBubbleColor : Theme.assistantBubbleColor
        anchors {
            left: isUser ? undefined : parent.left
            right: isUser ? parent.right : undefined
            leftMargin: isUser ? 0 : 16
            rightMargin: isUser ? 16 : 0
        }

        ColumnLayout {
            id: contentCol
            anchors.fill: parent
            anchors.margins: 12
            spacing: 4

            // Provider badge
            Row {
                id: badgeRow
                visible: !isUser && msgProviderName !== ""
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
                Label {
                    visible: msgModelName !== ""
                    text: "· " + msgModelName
                    color: Theme.tertiaryTextColor
                    font.pixelSize: Theme.fontSizeSmall
                }
            }

            // Content
            Label {
                id: contentLabel
                text: msgContent + (msgIsStreaming ? " ▌" : "")
                color: isUser ? "#ffffff" : Theme.primaryTextColor
                font.pixelSize: Theme.fontSizeBody
                wrapMode: Text.Wrap
                textFormat: Text.PlainText
                Layout.fillWidth: true
            }

            // Timestamp
            Label {
                text: msgTimestampFormatted
                color: Theme.tertiaryTextColor
                font.pixelSize: Theme.fontSizeSmall
                anchors.right: parent.right
            }
        }

        // Copy menu
        MouseArea {
            anchors.fill: parent
            onPressAndHold: contextMenu.popup()
        }

        Menu {
            id: contextMenu

            background: Rectangle {
                color: Theme.surfaceColor
                radius: 8
            }

            MenuItem {
                text: "Copy"
                onClicked: {
                    // Clipboard copy
                }
                contentItem: Label {
                    text: "Copy"
                    color: Theme.primaryTextColor
                }
            }
        }
    }

    NumberAnimation on opacity {
        id: fadeIn
        from: 0; to: 1
        duration: 200
        running: true
    }
}
