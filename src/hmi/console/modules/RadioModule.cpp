#include "ModuleUi.hpp"

#include <algorithm>
#include <stdexcept>

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderRadio(Canvas& c, Rect r, const ViewModel&, const ScreenData& d) {
    const auto& radio = d.radio;
    c.box(r, "RADIO TUNER");

    // Signal bars
    const int sig = std::clamp(radio.signal, 0, 5);
    std::string bars;
    for (int i = 0; i < 5; ++i) bars += (i < sig ? '|' : '.');
    const std::string sigStr = "[" + bars + "]";

    // Band + frequency line
    const bool isAM  = (radio.band == "AM");
    const std::string bandFreq = (radio.band.empty() ? "FM" : radio.band)
                               + "  "
                               + (radio.freq.empty() ? "87.5" : radio.freq)
                               + (isAM ? " kHz" : " MHz");
    c.text(r.x + 4, r.y + 2, bandFreq);

    // Status + signal on the right
    const std::string statusSig = (radio.status.empty() ? "idle" : radio.status) + "  " + sigStr;
    const int sigX = r.x + r.w - 2 - static_cast<int>(statusSig.size());
    if (sigX > r.x + 4) c.text(sigX, r.y + 2, statusSig);

    // Station name
    const std::string stationName = radio.name.empty() ? "(between stations)" : radio.name;
    c.text(r.x + 4, r.y + 3, stationName);

    if (!isAM) {
        // FM band scale
        c.text(r.x + 4, r.y + 5, "88     92     96     100    104    108");
        c.text(r.x + 4, r.y + 6, "|------|------|------|------|------|------|");

        // Frequency needle
        float freq = 87.5f;
        try { freq = std::stof(radio.freq.empty() ? "87.5" : radio.freq); } catch (...) {}
        const int pos = static_cast<int>(
            std::clamp((freq - 88.0f) / 20.0f, 0.0f, 1.0f) * 42.0f);
        std::string needle(43, ' ');
        needle[static_cast<size_t>(pos)] = '^';
        c.text(r.x + 4, r.y + 7, needle);
    } else {
        // AM band info
        c.text(r.x + 4, r.y + 5, "AM Band: 531 - 1602 kHz");
        c.text(r.x + 4, r.y + 6, "Tuned: " + (radio.freq.empty() ? "612" : radio.freq) + " kHz");
    }

    // Station preset reference card
    module_ui::card(c, Rect{r.x + 4, r.y + 9, r.w - 8, 7}, "STATIONS", {
        "P1: 87.5  Groove Salad       P2: 98.7  Underground 80s",
        "P3: 101.1 PopTron            P4: 104.3 Secret Agent",
        "P5: 107.9 Mission Control    P6: AM 612 The Trip",
        "[Seek+] [Seek-] [Scan] navigate bands and stations"
    });
}

HmiModule radioModule() {
    return HmiModule{Screen::Radio,
                     "radio",
                     "RADIO",
                     {},
                     {
                         // 0-4: FM presets
                         {"1", "87.5 Groove",  action(action_id::RadioTunerTune, {"87.5"})},
                         {"2", "98.7 Secret",  action(action_id::RadioTunerTune, {"98.7"})},
                         {"3", "101.1 Ill St", action(action_id::RadioTunerTune, {"101.1"})},
                         {"4", "103.7 Beat",   action(action_id::RadioTunerTune, {"103.7"})},
                         {"5", "107.9 Folk",   action(action_id::RadioTunerTune, {"107.9"})},
                         // 5: AM preset
                         {"6", "AM 612 Trip",  action(action_id::RadioTunerTune, {"AM:612"})},
                         // 6-9: controls
                         {"7", "Seek +",       action(action_id::RadioTunerSeek, {"up"})},
                         {"8", "Seek -",       action(action_id::RadioTunerSeek, {"down"})},
                         {"9", "Scan",         action(action_id::RadioTunerSeek, {"scan"})},
                         {"0", "Stop",         action(action_id::RadioTunerTune, {"stop"})},
                         // 10-11: band toggle
                         {"f", "FM Band",      action(action_id::RadioTunerTune, {"FM"})},
                         {"a", "AM Band",      action(action_id::RadioTunerTune, {"AM"})},
                     },
                     renderRadio};
}

} // namespace mini_hmi
