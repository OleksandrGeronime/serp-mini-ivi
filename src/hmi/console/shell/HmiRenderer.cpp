#include "HmiRenderer.hpp"

#include "HmiScreens.hpp"
#include "UiPrimitives.hpp"

#include <algorithm>

namespace mini_hmi {

RenderedFrame HmiRenderer::render(const ViewModel& model, int requestedWidth, int requestedHeight) const {
    // Keep one terminal column free. Printing full-width lines can trigger
    // terminal autowrap, which visually shifts rows away from mouse hit targets.
    Canvas canvas(std::max(1, requestedWidth - 1), requestedHeight);
    const auto data = screenDataFrom(model);

    canvas.statusBar(model, data.vehicle, data.focus);

    const int railWidth = 18;
    const int top = 4;
    const int bottom = 11;
    const int bodyHeight = canvas.height() - top - bottom;
    canvas.appRail(Rect{0, top, railWidth, bodyHeight}, model.screen);
    renderScreen(canvas, Rect{railWidth + 1, top, canvas.width() - railWidth - 1, bodyHeight}, model, data);

    const int messageY = canvas.height() - bottom;
    canvas.hline(0, messageY, canvas.width(), '=');
    canvas.text(2, messageY + 1, "Last action: " + (model.lastCommand.empty() ? std::string("<none>") : model.lastCommand));
    canvas.text(2, messageY + 2, "Message: " + clampText(model.message, canvas.width() - 12));
    canvas.softKeyStrip(model);

    return RenderedFrame{canvas.str(), canvas.targets()};
}

} // namespace mini_hmi
