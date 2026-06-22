#pragma once

#include "HmiTypes.hpp"
#include "HmiState.hpp"

#include <string>
#include <vector>

namespace mini_hmi {

struct Rect {
    int x{0};
    int y{0};
    int w{0};
    int h{0};
};

class Canvas {
public:
    Canvas(int width, int height);

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    void text(int x, int y, const std::string& value);
    void centered(int y, const std::string& value);
    void box(Rect rect, const std::string& title = {});
    void fill(Rect rect, char ch = ' ');
    void hline(int x, int y, int width, char ch = '-');
    void vline(int x, int y, int height, char ch = '|');
    void progress(Rect rect, int percent, const std::string& label = {});
    void button(Rect rect, const std::string& id, const std::string& label, bool enabled = true);
    void appRail(Rect rect, Screen active);
    void statusBar(const ViewModel& model, const VehicleState& vehicle, const AudioFocusState& focus);
    void softKeyStrip(const ViewModel& model);

    const std::vector<HitTarget>& targets() const { return mTargets; }
    std::string str() const;

private:
    void put(int x, int y, char ch);
    void addTarget(Rect rect, std::string id);

    int mWidth{0};
    int mHeight{0};
    std::vector<std::string> mRows;
    std::vector<HitTarget> mTargets;
};

std::string clampText(std::string value, int width);
int parsePercentLike(const std::string& value, int fallback = 0);
int parseIntLike(const std::string& value, int fallback = 0);

} // namespace mini_hmi
