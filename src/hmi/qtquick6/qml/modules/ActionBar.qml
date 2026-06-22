import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    color: "#161d23"
    border.color: "#2c3944"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            Label { text: "Last: " + hmi.lastAction; color: "#d7e0e6"; Layout.fillWidth: true; elide: Text.ElideRight }
            Label { text: hmi.message; color: "#8fd19e"; Layout.preferredWidth: 280; elide: Text.ElideRight }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 8
            columnSpacing: 8
            rowSpacing: 8
            Repeater {
                model: hmi.actions
                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 42
                    text: modelData.label
                    enabled: modelData.enabled
                    onClicked: hmi.triggerAction(modelData.index)
                }
            }
        }
    }
}
