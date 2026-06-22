#pragma once

#include "HmiTypes.hpp"
#include "HmiState.hpp"
#include "UiPrimitives.hpp"

namespace mini_hmi {

ScreenData screenDataFrom(const ViewModel& model);
void renderScreen(Canvas& canvas, Rect rect, const ViewModel& model, const ScreenData& data);

} // namespace mini_hmi
