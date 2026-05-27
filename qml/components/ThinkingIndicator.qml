import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: thinkingIndicator
    property bool active: visible

    height: visible ? 40 : 0
    color: "transparent"

    RowLayout {
        anchors.centerIn: parent
        spacing: 12

        Label {
            text: "Thinking"
            color: Theme.secondaryTextColor
            font.pixelSize: Theme.fontSizeSmall
        }

        Row {
            spacing: 4

            Repeater {
                model: 3

                Rectangle {
                    width: 6
                    height: 6
                    radius: 3
                    color: Theme.accentColor

                    SequentialAnimation on opacity {
                        running: thinkingIndicator.active
                        loops: Animation.Infinite

                        PauseAnimation { duration: index * 200 }
                        NumberAnimation { from: 0.2; to: 1.0; duration: 300 }
                        NumberAnimation { from: 1.0; to: 0.2; duration: 300 }
                        PauseAnimation { duration: (2 - index) * 200 }
                    }
                }
            }
        }
    }
}
