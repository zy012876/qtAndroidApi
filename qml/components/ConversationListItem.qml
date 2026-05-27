import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: listItem
    property string convId: ""
    property string convTitle: ""
    property string convProviderName: ""
    property string convPreview: ""
    property int convMessageCount: 0
    property string convUpdatedAtFormatted: ""

    width: parent ? parent.width : 340
    height: 72
    color: mouseArea.containsMouse ? Qt.lighter(Theme.surfaceColor, 1.2) : Theme.surfaceColor

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        Image {
            source: "qrc:/icons/" + (convProviderName || "openai") + ".svg"
            width: 24; height: 24
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2

            Label {
                text: convTitle || "New Chat"
                color: Theme.primaryTextColor
                font.pixelSize: Theme.fontSizeBody
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
            }

            Label {
                text: convPreview || "No messages"
                color: Theme.secondaryTextColor
                font.pixelSize: Theme.fontSizeSmall
                elide: Text.ElideRight
                Layout.fillWidth: true
                maximumLineCount: 1
            }
        }

        ColumnLayout {
            spacing: 2
            Label {
                text: convUpdatedAtFormatted
                color: Theme.tertiaryTextColor
                font.pixelSize: 10
            }
            Label {
                text: convMessageCount + " msgs"
                color: Theme.tertiaryTextColor
                font.pixelSize: 10
                horizontalAlignment: Text.AlignRight
                Layout.fillWidth: true
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            chatManager.switchConversation(convId)
            if (pageStack.depth > 1)
                pageStack.pop()
        }
    }
}
