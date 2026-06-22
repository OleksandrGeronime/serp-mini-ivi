// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "LocationHalBase.hpp"

namespace Minivi
{
    class LocationHal : public LocationHalBase
    {
    public:
        explicit LocationHal();
        ~LocationHal() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void calculateRoute(serp::ResponsePtr<std::string> reply, const std::string& destination) override;
        void nextManeuver(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;

    private:
        void onPositionTick();

        serp::TimerPtr mPositionTimer;
        std::chrono::steady_clock::time_point mStartTime;

        std::mutex mWritersMutex;
        std::vector<serp::StreamWriter<GpsPosition>> mStreamWriters;
    };

} // namespace Minivi
