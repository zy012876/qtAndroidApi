import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Row {
    id: chipRow
    spacing: 4

    Repeater {
        model: chatManager.registeredProviders.length > 0
               ? chatManager.registeredProviders
               : ["openai", "anthropic", "gemini"]

        delegate: ItemDelegate {
            width: chipContent.implicitWidth + 20
            height: 32
            highlighted: chatManager.currentProviderId === modelData

            contentItem: Row {
                id: chipContent
                spacing: 4
                Image {
                    source: "qrc:/icons/" + modelData + ".svg"
                    width: 16; height: 16
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    text: {
                        if (modelData === "openai") return "GPT"
                        if (modelData === "anthropic") return "Claude"
                        if (modelData === "gemini") return "Gemini"
                        if (modelData === "deepseek") return "DeepSeek"
                        return modelData
                    }
                    anchors.verticalCenter: parent.verticalCenter
                    color: highlighted ? "#ffffff" : Theme.secondaryTextColor
                    font.pixelSize: 12
                    font.weight: highlighted ? Font.DemiBold : Font.Normal
                }
            }

            background: Rectangle {
                radius: Theme.chipRadius
                color: highlighted ? Theme.accentColor : "transparent"
                border.color: highlighted ? Theme.accentColor : Theme.dividerColor
            }

            onClicked: chatManager.setCurrentProviderId(modelData)
        }
    }
}
