#pragma once

#include <string>
#include <vector>

namespace mini_hmi {

struct ContactItem {
    std::string name;
    std::string number;
};

struct RecentCallItem {
    std::string name;
    std::string number;
    std::string direction;
    std::string timestamp;
    std::string duration;
};

struct MediaState {
    std::string state{"stopped"};
    std::string source{"none"};
    std::string track{"No track"};
    std::string artist{"Unknown"};
    std::string trackIndex{"0"};
    std::string playlistSize{"0"};
};

struct RadioState {
    std::string station{"87.5"};
    std::string band{"FM"};
    std::string freq{"87.5"};
    std::string name{""};
    std::string status{"idle"};
    int signal{0};
};

struct AudioFocusState {
    std::string owner{"none"};
    std::string mode{"idle"};
};

struct NavigationState {
    std::string state{"idle"};
    std::string destination{"none"};
};

struct PhoneState {
    std::string state{"idle"};
    std::string number{};
    std::string contactName{};
    std::string direction{};
    bool        muteActive{false};
    std::string callDuration{};
    std::string activeTab{"favorites"};

    std::vector<ContactItem>    contacts;
    std::vector<ContactItem>    favoriteContacts;
    std::vector<RecentCallItem> recentCalls;
    std::vector<ContactItem>    dialFilteredContacts;
    std::string                 dialQuery{};
    std::vector<ContactItem>    contactSearchResults;
    std::string                 contactSearchQuery{};
};

struct ClimateState {
    std::string driver{"72"};
    std::string passenger{"70"};
    std::string fan{"30"};
    std::string ac{"off"};
    std::string sync{"off"};
    std::string autoMode{"off"};
    std::string frontDefrost{"off"};
    std::string rearDefrost{"off"};
};

struct ProfileState {
    std::string active{"default"};
};

struct VehicleState {
    std::string ignition{"off"};
    std::string gear{"park"};
    std::string speed{"0"};
    std::string moving{"false"};
};

struct SettingsState {
    std::string theme{"dark"};
    std::string units{"imperial"};
    std::string brightness{"80"};
    std::string layout{"split"};
};

struct GpsState {
    std::string latitude;
    std::string longitude;
    std::string speed;
    std::string heading;
};

struct ScreenData {
    MediaState media;
    RadioState radio;
    AudioFocusState focus;
    NavigationState nav;
    PhoneState phone;
    ClimateState climate;
    ProfileState profile;
    VehicleState vehicle;
    SettingsState settings;
    GpsState gps;
    std::vector<std::string> notifications;
};

} // namespace mini_hmi
