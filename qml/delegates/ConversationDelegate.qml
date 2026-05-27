import QtQuick
import "../components"
import "../dialogs"

ConversationListItem {
    convId: model.convId
    convTitle: model.convTitle
    convProviderName: model.convProviderName
    convPreview: model.convPreview
    convMessageCount: model.convMessageCount
    convUpdatedAtFormatted: model.convUpdatedAtFormatted

    // Swipe to delete
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onPressAndHold: deleteDialog.open()
    }

    DeleteConfirmDialog {
        id: deleteDialog
        onConfirmed: chatManager.deleteConversation(convId)
    }
}
