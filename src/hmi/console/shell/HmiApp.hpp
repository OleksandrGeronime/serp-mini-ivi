#pragma once

#include "HmiController.hpp"
#include "HmiRenderer.hpp"
#include "TerminalInput.hpp"

namespace mini_hmi {

class HmiApp {
public:
    explicit HmiApp(HmiController& controller);
    int runInteractive();
    int runScripted();

private:
    void handleEvent(const InputEvent& event, const std::vector<HitTarget>& hitTargets, bool& running);
    void moveScreen(int delta);

    HmiController& mController;
    HmiRenderer mRenderer;
};

} // namespace mini_hmi
