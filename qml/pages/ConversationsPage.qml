import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../delegates"
import "../dialogs"

Page {
    id: conversationsPage
    background: Rectangle { color: Theme.backgroundColor }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header
        Rectangle {
            Layout.fillWidth: true
            height: 56
            color: Theme.surfaceColor

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                ToolButton {
                    icon.source: "qrc:/icons/delete.svg"
                    icon.color: Theme.primaryTextColor
                    rotation: 180
                    onClicked: pageStack.pop()
                }

                Label {
                    text: "Conversation History"
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.primaryTextColor
                    Layout.fillWidth: true
                }

                ToolButton {
                    icon.source: "qrc:/icons/add.svg"
                    onClicked: pageStack.push("NewConversationDialog.qml")
                }
            }
        }

        // Conversation list
        ListView {
            id: convListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: chatManager.conversations
            delegate: ConversationDelegate {}
            spacing: 2

            Label {
                anchors.centerIn: parent
                text: "No conversations yet"
                color: Theme.secondaryTextColor
                visible: convListView.count === 0
            }
        }
    }
}
