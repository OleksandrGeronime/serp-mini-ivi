import QtQuick 6.0
import QtQuick.Controls 6.0
import QtQuick.Layouts 1.15
import "modules"

ApplicationWindow {
    id: root
    width: 1180
    height: 720
    visible: true
    title: "Minivi HMI"
    color: "#101418"

    property string currentScreen: "radio"

    RowLayout {
        anchors.fill: parent
        spacing: 0

        ListView {
            id: navList
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            model: ListModel {
                ListElement { screenId: "radio";         title: "Radio" }
                ListElement { screenId: "media";         title: "Media" }
                ListElement { screenId: "climate";       title: "Climate" }
                ListElement { screenId: "navigation";    title: "Navigation" }
                ListElement { screenId: "phone";         title: "Phone" }
                ListElement { screenId: "vehicle";       title: "Vehicle" }
                ListElement { screenId: "settings";      title: "Settings" }
                ListElement { screenId: "notifications"; title: "Notifications" }
            }
            delegate: Button {
                width: navList.width
                height: 48
                text: model.title
                highlighted: root.currentScreen === model.screenId
                onClicked: root.currentScreen = model.screenId
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: ["radio","media","climate","navigation","phone","vehicle","settings","notifications"].indexOf(root.currentScreen)
            RadioPane {}
            MediaPane {}
            ClimatePane {}
            NavigationPane {}
            PhonePane {}
            VehiclePane {}
            SettingsPane {}
            NotificationsPane {}
        }
    }
}
