import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    property bool profileMode: false
    property bool notificationsMode: false
    color: "#151b20"
    Label {
        anchors.centerIn: parent
        width: parent.width * 0.82
        color: "#d7e0e6"
        text: notificationsMode ? hmi.notifications : (profileMode ? "Active profile: " + hmi.profile : hmi.settingsLine)
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 28
        wrapMode: Text.WordWrap
    }
}
