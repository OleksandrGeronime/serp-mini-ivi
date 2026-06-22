import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    color: "#151b20"
    Label {
        anchors.centerIn: parent
        color: "#d7e0e6"
        text: "Vehicle\n" + hmi.vehicleLine
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 30
    }
}
