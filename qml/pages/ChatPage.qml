import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"
import "../delegates"
import "../dialogs"

Page {
    id: chatPage
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
                    icon.source: "qrc:/icons/history.svg"
                    onClicked: pageStack.push("ConversationsPage.qml")
                }

                ProviderChipSelector {
                    Layout.fillWidth: true
                }

                ToolButton {
                    icon.source: "qrc:/icons/settings.svg"
                    onClicked: pageStack.push("SettingsPage.qml")
                }
            }
        }

        // Model selector row
        Rectangle {
            Layout.fillWidth: true
            height: visible ? 40 : 0
            color: Theme.surfaceColor
            visible: chatManager.hasActiveConversation

            ComboBox {
                id: modelSelector
                anchors.centerIn: parent
                model: chatManager.availableModels(chatManager.currentProviderId)
                currentIndex: model.indexOf(chatManager.currentModel)
                onCurrentTextChanged: {
                    if (currentText !== chatManager.currentModel)
                        chatManager.setCurrentModel(currentText)
                }

                background: Rectangle {
                    color: Theme.cardColor
                    radius: 8
                }

                contentItem: Label {
                    text: modelSelector.currentText
                    color: Theme.primaryTextColor
                    font.pixelSize: Theme.fontSizeSmall
                    horizontalAlignment: Text.AlignHCenter
                }

                Connections {
                    target: chatManager
                    function onCurrentModelChanged() {
                        modelSelector.currentIndex = modelSelector.model.indexOf(chatManager.currentModel)
                    }
                    function onCurrentProviderIdChanged() {
                        modelSelector.model = chatManager.availableModels(chatManager.currentProviderId)
                        modelSelector.currentIndex = modelSelector.model.indexOf(chatManager.currentModel)
                    }
                }
            }
        }

        // Message list
        ListView {
            id: messageListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: chatManager.currentMessages
            delegate: MessageDelegate {}
            spacing: 8
            clip: true
            topMargin: 8
            bottomMargin: 8

            property bool autoScroll: true

            onContentYChanged: {
                autoScroll = (contentHeight - contentY - height) < 80
            }

            Connections {
                target: chatManager.currentMessages
                function onCountChanged() {
                    if (messageListView.autoScroll)
                        messageListView.positionViewAtEnd()
                }
            }

            // Empty state
            Label {
                anchors.centerIn: parent
                text: "Start a conversation\nby selecting a provider and typing below"
                color: Theme.secondaryTextColor
                font.pixelSize: Theme.fontSizeBody
                horizontalAlignment: Text.AlignHCenter
                visible: messageListView.count === 0
            }

            // Scroll to bottom button
            RoundButton {
                visible: !messageListView.autoScroll && messageListView.count > 0
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.margins: 16
                width: 40
                height: 40
                radius: 20
                icon.source: "qrc:/icons/delete.svg"
                icon.color: Theme.backgroundColor
                palette.button: Theme.accentColor
                onClicked: messageListView.positionViewAtEnd()
                scale: 0.8
            }
        }

        // Thinking indicator
        ThinkingIndicator {
            Layout.fillWidth: true
            visible: chatManager.isStreaming && !hasContent()

            function hasContent() {
                if (chatManager.currentMessages.count === 0) return false
                var lastIdx = chatManager.currentMessages.count - 1
                var lastContent = chatManager.currentMessages.data(
                    chatManager.currentMessages.index(lastIdx, 0),
                    Qt.UserRole + 3  // ContentRole
                )
                return lastContent && lastContent.length > 0
            }
        }

        // Input bar
        ChatInputBar {
            Layout.fillWidth: true
            enabled: true
            isStreaming: chatManager.isStreaming
            onSendMessage: function(text) {
                chatManager.sendMessage(text)
            }
            onCancelRequest: chatManager.cancelCurrentRequest()
        }
    }
}
