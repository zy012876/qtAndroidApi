import QtQuick
import "../components"

Loader {
    id: messageDelegate
    property var modelData: model

    sourceComponent: {
        if (msgIsStreaming && msgRole === "assistant") {
            return streamingComponent
        }
        return standardComponent
    }

    Component {
        id: standardComponent
        MessageBubble {
            msgRole: model.msgRole
            msgContent: model.msgContent
            msgProviderName: model.msgProviderName
            msgModelName: model.msgModelName
            msgTimestampFormatted: model.msgTimestampFormatted
            msgIsStreaming: false
        }
    }

    Component {
        id: streamingComponent
        StreamingMessageBubble {
            msgContent: model.msgContent
            msgProviderName: model.msgProviderName
            msgModelName: model.msgModelName
            msgTimestampFormatted: model.msgTimestampFormatted
            msgIsStreaming: true
        }
    }
}
