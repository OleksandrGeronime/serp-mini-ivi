#include "ModuleUi.hpp"

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderMedia(Canvas& c, Rect r, const ViewModel&, const ScreenData& d) {
    const std::string state = d.media.state;
    const std::string track = d.media.track;
    c.box(Rect{r.x, r.y, 28, 14}, "ALBUM ART");
    c.text(r.x + 5, r.y + 3, "  ____________");
    c.text(r.x + 5, r.y + 4, " /            \\");
    c.text(r.x + 5, r.y + 5, "|   SERP FM    |");
    c.text(r.x + 5, r.y + 6, "|  MEDIA BUS   |");
    c.text(r.x + 5, r.y + 7, "|      >>      |");
    c.text(r.x + 5, r.y + 8, " \\____________/");
    module_ui::card(c, Rect{r.x + 30, r.y, r.w - 30, 8}, "PLAYER", {
        "Source: " + d.media.source,
        "Track : " + track,
        "State : " + state,
        "Audio : " + d.focus.owner + " / " + d.focus.mode});
    c.progress(Rect{r.x + 30, r.y + 9, r.w - 30, 3}, state == "playing" ? 42 : 0, "TRACK POSITION");
    module_ui::card(c, Rect{r.x + 30, r.y + 13, r.w - 30, 5}, "QUEUE", {"> " + track, "  Downtown Drive", "  Evening Podcast"});
}

HmiModule mediaModule() {
    return HmiModule{Screen::Media,
                     "media",
                     "MEDIA",
                     {},
                     {{"1", "Play", action(action_id::MediaSessionPlay, {"demo_playlist"})}, {"2", "Pause", action(action_id::MediaSessionPause)}, {"3", "Next", action(action_id::MediaSessionSkipNext)}, {"4", "Stop", action(action_id::MediaSessionStop)},
                      {"5", "Roadtrip", action(action_id::MediaSessionPlay, {"roadtrip"})}, {"6", "Podcast", action(action_id::MediaSessionPlay, {"podcast"})}, {"7", "Vol 45", action(action_id::CarAudioSetVolume, {"cabin", "45"})}, {"8", "Vol 70", action(action_id::CarAudioSetVolume, {"cabin", "70"})}},
                     renderMedia};
}

} // namespace mini_hmi
