import QtQuick 2.15
import SolTraceProto

Text {
    textFormat: Text.RichText
    color: Theme.textColor
    font.pointSize: 16
    wrapMode: Text.WordWrap

    property string rawText: ""

    text: renderLatex(rawText)

    function renderLatex(text) {
        return text.replace(/\$([^\$]+)\$/g, (match, expr) => {
            return `<span style='font-family: "CMU Serif"; font-weight: bold;
                                  font-style: italic;'>${expr}</span>`
        })
    }
}
