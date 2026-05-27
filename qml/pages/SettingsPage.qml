import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: settingsPage
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
                    text: "Settings"
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.primaryTextColor
                    Layout.fillWidth: true
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                width: parent.width
                spacing: 4
                anchors.margins: Theme.pageMargin

                // API Keys section
                Label {
                    text: "API Keys"
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.primaryTextColor
                    Layout.margins: Theme.pageMargin
                    Layout.topMargin: Theme.pageMargin
                }

                Repeater {
                    model: ["openai", "anthropic", "gemini", "deepseek"]

                    delegate: Rectangle {
                        Layout.fillWidth: true
                        height: 56
                        color: Theme.surfaceColor
                        radius: Theme.bubbleRadius

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 12

                            Image {
                                source: "qrc:/icons/" + modelData + ".svg"
                                width: 28; height: 28
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Label {
                                    text: {
                                        if (modelData === "openai") return "OpenAI"
                                        if (modelData === "anthropic") return "Anthropic Claude"
                                        if (modelData === "gemini") return "Google Gemini"
                                        if (modelData === "deepseek") return "DeepSeek"
                                        return modelData
                                    }
                                    color: Theme.primaryTextColor
                                    font.pixelSize: Theme.fontSizeBody
                                }
                                Label {
                                    text: settingsManager.hasApiKey(modelData)
                                          ? "API Key: ****" + settingsManager.loadApiKey(modelData).slice(-4)
                                          : "No API key"
                                    color: Theme.secondaryTextColor
                                    font.pixelSize: Theme.fontSizeSmall
                                }
                            }

                            ToolButton {
                                text: settingsManager.hasApiKey(modelData) ? "Edit" : "Add"
                                onClicked: {
                                    var page = pageStack.push("ApiKeyManagementPage.qml",
                                        {"providerId": modelData})
                                }
                            }
                        }
                    }
                }

                // Preferences section
                Label {
                    text: "Preferences"
                    font.pixelSize: Theme.fontSizeTitle
                    font.bold: true
                    color: Theme.primaryTextColor
                    Layout.margins: Theme.pageMargin
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 48
                    color: Theme.surfaceColor
                    radius: Theme.bubbleRadius

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        Label {
                            text: "Dark Mode"
                            color: Theme.primaryTextColor
                            Layout.fillWidth: true
                        }
                        Switch {
                            checked: settingsManager.darkMode()
                            onToggled: settingsManager.setDarkMode(checked)
                        }
                    }
                }
            }
        }
    }
}
