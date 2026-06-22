import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 1.15
import QtLocation 6.0
import QtPositioning 6.0

Item {
    Layout.fillWidth: true
    Layout.fillHeight: true

    readonly property real currentLat: isFinite(iCarNavigationManager.latitude) ? iCarNavigationManager.latitude : 42.3314
    readonly property real currentLon: isFinite(iCarNavigationManager.longitude) ? iCarNavigationManager.longitude : -83.0458
    readonly property int trailLimit: 80
    property bool followPosition: true
    property bool headingUp: false
    property bool updatingMapCenter: false
    property var trailCoordinates: []

    function currentCoordinate() {
        return QtPositioning.coordinate(currentLat, currentLon)
    }

    function recenterMap() {
        followPosition = true
        updatingMapCenter = true
        map.center = currentCoordinate()
        updatingMapCenter = false
    }

    function appendTrailPoint() {
        var point = currentCoordinate()
        if (!point.isValid) {
            return
        }

        var next = trailCoordinates.slice()
        if (next.length > 0) {
            var last = next[next.length - 1]
            if (Math.abs(last.latitude - point.latitude) < 0.000001
                    && Math.abs(last.longitude - point.longitude) < 0.000001) {
                return
            }
        }

        next.push(point)
        while (next.length > trailLimit) {
            next.shift()
        }
        trailCoordinates = next
    }

    function handlePositionChanged() {
        appendTrailPoint()
        followPositionTimer.restart()
    }

    onCurrentLatChanged: handlePositionChanged()
    onCurrentLonChanged: handlePositionChanged()

    Component.onCompleted: appendTrailPoint()

    Map {
        id: map
        anchors.fill: parent

        plugin: Plugin {
            name: "osm"
            PluginParameter { name: "osm.useragent"; value: "SerpMinivi/1.0" }
            PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: true }
            PluginParameter { name: "osm.mapping.custom.host"; value: "https://tile.openstreetmap.org/" }
            PluginParameter { name: "osm.mapping.custom.mapcopyright"; value: "OpenStreetMap contributors" }
        }

        center: QtPositioning.coordinate(currentLat, currentLon)
        zoomLevel: 15
        bearing: headingUp ? iCarNavigationManager.heading : 0
        copyrightsVisible: false

        onCenterChanged: {
            if (!updatingMapCenter && !followPositionTimer.running) {
                followPosition = false
            }
        }

        MapPolyline {
            id: positionTrail
            path: trailCoordinates
            line.width: 4
            line.color: "#2db7ff"
            opacity: 0.7
        }

        Component.onCompleted: {
            for (var i = 0; i < supportedMapTypes.length; i++) {
                if (supportedMapTypes[i].name === "Custom URL Map") {
                    activeMapType = supportedMapTypes[i]
                    break
                }
            }
        }

        MapQuickItem {
            id: positionMarker
            coordinate: QtPositioning.coordinate(currentLat, currentLon)
            anchorPoint.x: markerRoot.width / 2
            anchorPoint.y: markerRoot.height / 2
            sourceItem: Item {
                id: markerRoot
                width: 36
                height: 36

                Rectangle {
                    width: 7
                    height: 16
                    radius: 4
                    color: "#1687ff"
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    transform: Rotation {
                        origin.x: 3.5
                        origin.y: 22
                        angle: iCarNavigationManager.heading
                    }
                }

                Rectangle {
                    width: 20
                    height: 20
                    radius: 10
                    color: "#1687ff"
                    border.color: "white"
                    border.width: 3
                    anchors.centerIn: parent
                }
            }
        }
    }

    Timer {
        id: followPositionTimer
        interval: 1
        repeat: false
        onTriggered: {
            if (followPosition) {
                updatingMapCenter = true
                map.center = currentCoordinate()
                updatingMapCenter = false
            }
        }
    }

    ColumnLayout {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 12
        spacing: 8

        Button {
            Layout.preferredWidth: 38
            Layout.preferredHeight: 34
            text: "+"
            onClicked: map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 1)
        }

        Button {
            Layout.preferredWidth: 38
            Layout.preferredHeight: 34
            text: "-"
            onClicked: map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 1)
        }

        Button {
            Layout.preferredWidth: 38
            Layout.preferredHeight: 34
            text: "C"
            onClicked: recenterMap()
        }

        Button {
            Layout.preferredWidth: 38
            Layout.preferredHeight: 34
            checkable: true
            checked: headingUp
            text: "N"
            onClicked: headingUp = checked
        }
    }

    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 12
        width: Math.min(260, parent.width - 24)
        color: "#d80f1720"
        radius: 10
        border.color: "#33465a"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 14
            spacing: 12

            Label {
                text: "NAVIGATION"
                color: "#9eb6c8"
                font.pixelSize: 13
                font.letterSpacing: 3
            }

            Rectangle {
                Layout.fillWidth: true
                height: 56
                color: iCarNavigationManager.routeActive ? "#17304a" : "#1c2530"
                radius: 8
                border.color: iCarNavigationManager.routeActive ? "#2f75b5" : "#2c3944"
                Label {
                    anchors.centerIn: parent
                    width: parent.width - 20
                    text: iCarNavigationManager.routeActive ? iCarNavigationManager.destination : "No active route"
                    color: "white"
                    font.pixelSize: 15
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                TextField {
                    id: destField
                    Layout.fillWidth: true
                    placeholderText: "Enter destination"
                    color: "white"
                    background: Rectangle {
                        color: "#1c2530"
                        radius: 8
                        border.color: "#2c3944"
                    }
                }
                Button {
                    text: "Go"
                    enabled: !iCarNavigationManager.routeActive
                    onClicked: { if (destField.text) iCarNavigationManager.startRoute(destField.text) }
                }
            }

            Button {
                Layout.fillWidth: true
                text: "Cancel"
                enabled: iCarNavigationManager.routeActive
                onClicked: iCarNavigationManager.cancelRoute()
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                columnSpacing: 14
                rowSpacing: 7

                Label { text: "Latitude"; color: "#8096a6"; font.pixelSize: 12 }
                Label { text: currentLat.toFixed(6); color: "white"; font.pixelSize: 12 }
                Label { text: "Longitude"; color: "#8096a6"; font.pixelSize: 12 }
                Label { text: currentLon.toFixed(6); color: "white"; font.pixelSize: 12 }
                Label { text: "Speed"; color: "#8096a6"; font.pixelSize: 12 }
                Label { text: iCarNavigationManager.speed.toFixed(1) + " km/h"; color: "white"; font.pixelSize: 12 }
                Label { text: "Heading"; color: "#8096a6"; font.pixelSize: 12 }
                Label { text: iCarNavigationManager.heading.toFixed(1) + " deg"; color: "white"; font.pixelSize: 12 }
                Label { text: "Trail"; color: "#8096a6"; font.pixelSize: 12 }
                Label { text: trailCoordinates.length + " points"; color: "white"; font.pixelSize: 12 }
            }

            Label {
                visible: iCarNavigationManager.routeActive
                Layout.fillWidth: true
                text: iCarNavigationManager.distanceToTarget.toFixed(1) + " km remaining"
                color: "#6db0ff"
                font.pixelSize: 13
                elide: Text.ElideRight
            }

            Item { Layout.fillHeight: true }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 12
        width: Math.min(240, parent.width - 24)
        height: 32
        radius: 8
        color: "#cc0f1720"
        border.color: "#263746"

        Label {
            anchors.centerIn: parent
            text: "Detroit simulation"
            color: "#9eb6c8"
            font.pixelSize: 12
        }
    }
}
