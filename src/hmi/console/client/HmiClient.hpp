#pragma once

#include "HmiActions.hpp"
#include "HmiState.hpp"

#include <string>

namespace mini_hmi {

class IHmiClient {
public:
    virtual ~IHmiClient() = default;

    virtual ScreenData snapshot() = 0;
    virtual std::string dispatch(const std::string& user, const HmiAction& action) = 0;
};

} // namespace mini_hmi
