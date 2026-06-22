#pragma once

#include "HmiTypes.hpp"

#include <string>
#include <vector>

namespace mini_hmi {

struct RenderedFrame {
    std::string text;
    std::vector<HitTarget> hitTargets;
};

class HmiRenderer {
public:
    RenderedFrame render(const ViewModel& model, int width = 120, int height = 36) const;
};

} // namespace mini_hmi
