import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: "Delete Conversation"
    standardButtons: Dialog.Yes | Dialog.No
    modal: true
    anchors.centerIn: parent

    signal confirmed()

    background: Rectangle {
        color: Theme.surfaceColor
        radius: 12
    }

    Label {
        text: "Are you sure you want to delete this conversation?\nThis action cannot be undone."
        color: Theme.primaryTextColor
        font.pixelSize: Theme.fontSizeBody
        wrapMode: Text.Wrap
        width: 280
    }

    onAccepted: confirmed()
}
