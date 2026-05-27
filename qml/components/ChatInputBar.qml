import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: inputBar
    property bool isStreaming: false

    signal sendMessage(string text)
    signal cancelRequest()

    height: 64
    color: Theme.inputBarColor
    border.color: Theme.dividerColor

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        TextArea {
            id: messageInput
            Layout.fillWidth: true
            Layout.fillHeight: true
            placeholderText: isStreaming ? "Waiting for response..." : "Type a message..."
            placeholderTextColor: Theme.tertiaryTextColor
            color: Theme.primaryTextColor
            font.pixelSize: Theme.fontSizeBody
            wrapMode: TextInput.Wrap
            enabled: !isStreaming

            background: Rectangle {
                color: Theme.backgroundColor
                radius: Theme.bubbleRadius
            }

            Keys.onReturnPressed: function(event) {
                if (text.trim() && !isStreaming) {
                    sendMessage(text)
                    text = ""
                }
            }
        }

        Loader {
            Layout.preferredWidth: 44
            Layout.preferredHeight: 44
            sourceComponent: isStreaming ? cancelButtonComponent : sendButtonComponent
        }
    }

    Component {
        id: sendButtonComponent
        RoundButton {
            id: sendButton
            enabled: messageInput.text.trim().length > 0 && !isStreaming
            icon.source: "qrc:/icons/send.svg"
            palette.button: enabled ? Theme.accentColor : Theme.dividerColor

            onClicked: {
                if (messageInput.text.trim()) {
                    inputBar.sendMessage(messageInput.text)
                    messageInput.text = ""
                }
            }
        }
    }

    Component {
        id: cancelButtonComponent
        RoundButton {
            icon.source: "qrc:/icons/delete.svg"
            palette.button: Theme.accentColor

            onClicked: inputBar.cancelRequest()
        }
    }
}
