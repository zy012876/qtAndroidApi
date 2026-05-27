import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: "API Key Required"
    standardButtons: Dialog.Ok
    modal: true
    anchors.centerIn: parent

    property string providerName: ""

    background: Rectangle {
        color: Theme.surfaceColor
        radius: 12
    }

    ColumnLayout {
        spacing: 12
        width: 280

        Label {
            text: "An API key for " + providerName + " is required.\nGo to Settings to add one."
            color: Theme.primaryTextColor
            font.pixelSize: Theme.fontSizeBody
            wrapMode: Text.Wrap
            Layout.fillWidth: true
        }

        Button {
            text: "Open Settings"
            Layout.fillWidth: true
            onClicked: {
                dialog.accept()
                pageStack.push("SettingsPage.qml")
            }

            background: Rectangle {
                color: Theme.accentColor
                radius: 8
            }
            contentItem: Label {
                text: "Open Settings"
                color: "white"
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
