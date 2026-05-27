import QtQuick
import QtQuick.Controls

Label {
    id: markdownLabel
    property string markdownText: text

    textFormat: Text.PlainText
    wrapMode: Text.Wrap
    font.pixelSize: Theme.fontSizeBody
    color: Theme.primaryTextColor

    onMarkdownTextChanged: {
        // Basic markdown: strip common formatting
        var txt = markdownText
        // Remove bold markers
        txt = txt.replace(/\*\*(.*?)\*\*/g, "$1")
        // Remove italic markers
        txt = txt.replace(/\*(.*?)\*/g, "$1")
        // Remove code block markers
        txt = txt.replace(/```[\s\S]*?```/g, "[code]")
        // Remove inline code markers
        txt = txt.replace(/`(.*?)`/g, "$1")
        text = txt
    }
}
