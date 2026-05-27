import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "pages"
import "dialogs"

ApplicationWindow {
    id: appWindow
    visible: true
    width: 400
    height: 700
    title: qsTr("AI Chat")
    color: Theme.backgroundColor

    StackView {
        id: pageStack
        anchors.fill: parent
        initialItem: ChatPage {}
    }

    // Error dialog
    Dialog {
        id: errorDialog
        title: "Error"
        standardButtons: Dialog.Ok
        modal: true
        anchors.centerIn: parent

        property string errorTitle: ""
        property string errorMessage: ""

        ColumnLayout {
            spacing: 8
            Label {
                text: errorDialog.errorTitle
                font.bold: true
                color: Theme.accentColor
            }
            Label {
                text: errorDialog.errorMessage
                color: Theme.primaryTextColor
                Layout.maximumWidth: 300
                wrapMode: Text.Wrap
            }
        }

        onAboutToShow: {
            // Update content
        }
    }

    Connections {
        target: chatManager
        function onErrorOccurred(title, message) {
            errorDialog.errorTitle = title
            errorDialog.errorMessage = message
            errorDialog.open()
        }
    }
}
