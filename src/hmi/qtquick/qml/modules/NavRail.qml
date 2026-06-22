import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    color: "#11171c"
    border.color: "#26333d"

    ListView {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 6
        model: hmi.navItems
        delegate: Button {
            width: ListView.view.width
            height: 42
            text: modelData.title
            checked: modelData.active
            checkable: false
            onClicked: hmi.openScreen(modelData.id)
        }
    }
}
