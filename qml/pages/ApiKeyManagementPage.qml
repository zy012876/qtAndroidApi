import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: apiKeyPage
    background: Rectangle { color: Theme.backgroundColor }

    property string providerId: "openai"

    function providerName() {
        if (providerId === "openai") return "OpenAI"
        if (providerId === "anthropic") return "Anthropic Claude"
        if (providerId === "gemini") return "Google Gemini"
        if (providerId === "deepseek") return "DeepSeek"
        return ""
    }

    function getKeyUrl() {
        if (providerId === "openai") return "https://platform.openai.com/api-keys"
        if (providerId === "anthropic") return "https://console.anthropic.com/keys"
        if (providerId === "gemini") return "https://aistudio.google.com/apikey"
        if (providerId === "deepseek") return "https://platform.deepseek.com/api_keys"
        return ""
    }

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
                    text: apiKeyPage.providerName() + " API Key"
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.primaryTextColor
                    Layout.fillWidth: true
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.margins: Theme.pageMargin
            spacing: 16

            Image {
                source: "qrc:/icons/" + providerId + ".svg"
                width: 48; height: 48
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                text: "Enter your " + apiKeyPage.providerName() + " API key"
                color: Theme.primaryTextColor
                font.pixelSize: Theme.fontSizeBody
                Layout.alignment: Qt.AlignHCenter
            }

            TextField {
                id: keyInput
                Layout.fillWidth: true
                placeholderText: "sk-..."
                echoMode: TextInput.Password
                color: Theme.primaryTextColor
                placeholderTextColor: Theme.tertiaryTextColor

                background: Rectangle {
                    color: Theme.surfaceColor
                    radius: Theme.bubbleRadius
                    border.color: Theme.dividerColor
                }

                Component.onCompleted: {
                    var existingKey = settingsManager.loadApiKey(providerId)
                    if (existingKey)
                        keyInput.text = existingKey
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8

                Button {
                    text: "Save"
                    Layout.fillWidth: true
                    onClicked: {
                        if (keyInput.text.trim()) {
                            settingsManager.saveApiKey(providerId, keyInput.text.trim())
                            pageStack.pop()
                        }
                    }

                    background: Rectangle {
                        color: Theme.accentColor
                        radius: 8
                    }
                    contentItem: Label {
                        text: "Save"
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }

                Button {
                    text: "Delete"
                    Layout.fillWidth: true
                    visible: settingsManager.hasApiKey(providerId)
                    onClicked: {
                        settingsManager.deleteApiKey(providerId)
                        pageStack.pop()
                    }

                    background: Rectangle {
                        color: "transparent"
                        border.color: Theme.accentColor
                        radius: 8
                    }
                    contentItem: Label {
                        text: "Delete"
                        color: Theme.accentColor
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            Label {
                text: "Get your API key at:\n" + getKeyUrl()
                color: Theme.secondaryTextColor
                font.pixelSize: Theme.fontSizeSmall
                wrapMode: Text.Wrap
                Layout.fillWidth: true
            }
        }
    }
}
