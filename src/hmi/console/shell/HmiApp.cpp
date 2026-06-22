#include "HmiApp.hpp"

#include "HmiModules.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace mini_hmi {

HmiApp::HmiApp(HmiController& controller)
    : mController(controller) {}

void HmiApp::moveScreen(int delta) {
    auto model = mController.viewModel();
    const auto& modules = hmiModules();
    auto it = std::find_if(modules.begin(), modules.end(), [&model](const HmiModule& module) { return module.screen == model.screen; });
    int index = it == modules.end() ? 0 : static_cast<int>(std::distance(modules.begin(), it));
    index = (index + delta + static_cast<int>(modules.size())) % static_cast<int>(modules.size());
    mController.switchScreen(modules[static_cast<std::size_t>(index)].screen);
}

void HmiApp::handleEvent(const InputEvent& event, const std::vector<HitTarget>& hitTargets, bool& running) {
    switch (event.type) {
        case InputEvent::Type::Quit:
            running = false;
            return;
        case InputEvent::Type::Digit:
            mController.pressButton(std::string(1, event.digit));
            return;
        case InputEvent::Type::DriverSeat:
            mController.switchSeat(Seat::Driver);
            return;
        case InputEvent::Type::CoDriverSeat:
            mController.switchSeat(Seat::CoDriver);
            return;
        case InputEvent::Type::Left:
            moveScreen(-1);
            return;
        case InputEvent::Type::Right:
            moveScreen(1);
            return;
        case InputEvent::Type::Up:
            mController.switchSeat(Seat::Driver);
            return;
        case InputEvent::Type::Down:
            mController.switchSeat(Seat::CoDriver);
            return;
        case InputEvent::Type::MouseClick:
            for (const auto& target : hitTargets) {
                if (event.mouseX >= target.x && event.mouseX < target.x + target.width && event.mouseY >= target.y && event.mouseY < target.y + target.height) {
                    if (target.buttonId.rfind("screen ", 0) == 0) {
                        mController.switchScreen(parseScreen(target.buttonId.substr(7)));
                    } else {
                        mController.pressButton(target.buttonId);
                    }
                    return;
                }
            }
            return;
        case InputEvent::Type::Refresh:
        case InputEvent::Type::Enter:
        case InputEvent::Type::None:
            return;
    }
}

int HmiApp::runInteractive() {
    using clock = std::chrono::steady_clock;

    TerminalSession terminal;
    bool running = true;
    bool dirty = true;
    auto nextRefresh = clock::now();
    std::vector<HitTarget> hitTargets;

    while (running) {
        const auto now = clock::now();
        if (dirty || now >= nextRefresh) {
            const auto frame = mRenderer.render(mController.viewModel(), terminal.width(), terminal.height());
            hitTargets = frame.hitTargets;
            terminal.draw(frame.text);
            dirty = false;
            nextRefresh = now + std::chrono::milliseconds(1500);
        }

        if (auto event = terminal.pollEvent(80)) {
            handleEvent(*event, hitTargets, running);
            dirty = true;
        }
    }
    return 0;
}

int HmiApp::runScripted() {
    std::vector<ViewModel> frames;
    mController.runScriptedDemo(frames);
    for (const auto& frameModel : frames) {
        const auto frame = mRenderer.render(frameModel, 120, 36);
        std::cout << frame.text << "\n";
    }
    return 0;
}

} // namespace mini_hmi
