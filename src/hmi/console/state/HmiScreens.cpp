#include "HmiScreens.hpp"

#include "HmiModules.hpp"

namespace mini_hmi {

ScreenData screenDataFrom(const ViewModel& model) {
    return model.state;
}

void renderScreen(Canvas& canvas, Rect rect, const ViewModel& model, const ScreenData& data) {
    const auto* module = hmiModule(model.screen);
    if (!module || !module->render) module = hmiModule(Screen::Home);
    if (module && module->render) module->render(canvas, rect, model, data);
}

} // namespace mini_hmi
