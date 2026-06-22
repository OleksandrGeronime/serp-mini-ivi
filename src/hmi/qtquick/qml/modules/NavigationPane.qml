import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtLocation 5.12
import QtPositioning 5.12

Item {
    // ── OSM map — fills the whole pane ───────────────────────────────────────
    Map {
        id: map
        anchors.fill: parent

        plugin: Plugin {
            name: "osm"
            PluginParameter { name: "osm.useragent";                            value: "SerpMinivi/1.0" }
            PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: true }
            PluginParameter { name: "osm.mapping.custom.host";                  value: "https://tile.openstreetmap.org/" }
            PluginParameter { name: "osm.mapping.custom.mapcopyright";          value: "© OpenStreetMap contributors" }
        }

        center: QtPositioning.coordinate(
            hmi.navLatitude  !== "" ? parseFloat(hmi.navLatitude)  : 42.3314,
            hmi.navLongitude !== "" ? parseFloat(hmi.navLongitude) : -83.0458
        )
        zoomLevel: 15
        copyrightsVisible: false

        Component.onCompleted: {
            for (var i = 0; i < supportedMapTypes.length; i++) {
                if (supportedMapTypes[i].name === "Custom URL Map") {
                    activeMapType = supportedMapTypes[i]
                    break
                }
            }
        }

        // Position dot with heading indicator
        MapQuickItem {
            id: positionMarker
            coordinate: map.center
            anchorPoint.x: markerRoot.width  / 2
            anchorPoint.y: markerRoot.height / 2
            sourceItem: Item {
                id: markerRoot
                width: 32; height: 32
                // Heading arrow
                Rectangle {
                    width: 6; height: 14; radius: 3
                    color: "#2196F3"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    transform: Rotation {
                        origin.x: 3; origin.y: 20
                        angle: hmi.navHeading !== "" ? parseFloat(hmi.navHeading) : 0
                    }
                }
                // Blue dot
                Rectangle {
                    width: 18; height: 18; radius: 9
                    color: "#2196F3"
                    border.color: "white"; border.width: 2
                    anchors.centerIn: parent
                }
            }
        }
    }

    // ── Guidance overlay panel (right side) ──────────────────────────────────
    Rectangle {
        anchors.right:  parent.right
        anchors.top:    parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 10
        width: 210
        color: "#cc0d1a2a"
        radius: 10
        border.color: "#40446677"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 10

            Label {
                text: "NAVIGATION"
                color: "#7ab0d8"
                font.pixelSize: 11
                font.bold: true
                font.letterSpacing: 2
            }

            Rectangle {
                Layout.fillWidth: true
                height: 36
                radius: 6
                color: hmi.navigationState === "active" ? "#0d2a0d" : "#0d1a0d"
                border.color: hmi.navigationState === "active" ? "#3a7a3a" : "#1a2a1a"
                border.width: 1
                Label {
                    anchors.centerIn: parent
                    text: hmi.navigationState.toUpperCase()
                    color: hmi.navigationState === "active" ? "#00cc44" : "#304030"
                    font.pixelSize: 13
                    font.bold: true
                }
            }

            ColumnLayout {
                spacing: 3
                Label { text: "DESTINATION"; color: "#446688"; font.pixelSize: 9; font.bold: true; font.letterSpacing: 1 }
                Label {
                    Layout.fillWidth: true
                    text: hmi.navigationDestination !== "" && hmi.navigationDestination !== "none"
                          ? hmi.navigationDestination : "(none)"
                    color: "#c8d8e8"
                    font.pixelSize: 12
                    wrapMode: Text.Wrap
                }
            }

            ColumnLayout {
                spacing: 3
                Label { text: "POSITION"; color: "#446688"; font.pixelSize: 9; font.bold: true; font.letterSpacing: 1 }
                Label {
                    text: hmi.navLatitude !== "" ? hmi.navLatitude : "—"
                    color: "#44cc88"; font.pixelSize: 11; font.family: "monospace"
                }
                Label {
                    text: hmi.navLongitude !== "" ? hmi.navLongitude : "—"
                    color: "#44cc88"; font.pixelSize: 11; font.family: "monospace"
                }
            }

            ColumnLayout {
                spacing: 3
                Label { text: "SPEED / HDG"; color: "#446688"; font.pixelSize: 9; font.bold: true; font.letterSpacing: 1 }
                Label {
                    text: (hmi.navSpeed !== "" ? hmi.navSpeed + " m/s" : "—")
                        + "   " + (hmi.navHeading !== "" ? hmi.navHeading + "°" : "—")
                    color: "#c8d8e8"; font.pixelSize: 11
                }
            }

            Item { Layout.fillHeight: true }

            Label {
                text: "Detroit simulation"
                color: "#1a3a2a"; font.pixelSize: 9; font.italic: true
            }
        }
    }
}
