import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: "New Conversation"
    standardButtons: Dialog.Ok | Dialog.Cancel
    modal: true
    anchors.centerIn: parent

    background: Rectangle {
        color: Theme.surfaceColor
        radius: 12
    }

    ColumnLayout {
        spacing: 12
        width: 280

        Label {
            text: "Select a provider for the new conversation"
            color: Theme.secondaryTextColor
            font.pixelSize: Theme.fontSizeSmall
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        ComboBox {
            id: providerCombo
            Layout.fillWidth: true
            model: chatManager.registeredProviders.length > 0
                   ? chatManager.registeredProviders
                   : ["openai", "anthropic", "gemini"]
            currentIndex: 0

            background: Rectangle {
                color: Theme.backgroundColor
                radius: 8
            }
            contentItem: Label {
                text: {
                    var val = providerCombo.currentText
                    if (val === "openai") return "OpenAI"
                    if (val === "anthropic") return "Anthropic Claude"
                    if (val === "gemini") return "Google Gemini"
                    if (val === "deepseek") return "DeepSeek"
                    return val
                }
                color: Theme.primaryTextColor
                verticalAlignment: Text.AlignVCenter
            }
        }

        onAccepted: {
            chatManager.createNewConversation(providerCombo.currentText)
            if (pageStack.depth > 1)
                pageStack.pop()
        }
    }
}
