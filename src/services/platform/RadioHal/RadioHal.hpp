// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <sys/types.h>

#include "RadioHalBase.hpp"

namespace Minivi
{
    class RadioHal : public RadioHalBase
    {
    public:
        explicit RadioHal();
        ~RadioHal() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void tune(serp::ResponsePtr<std::string> reply, const std::string& frequency) override;
        void seek(serp::ResponsePtr<std::string> reply, const std::string& direction) override;
        void frame(serp::ResponsePtr<std::string> reply) override;

    private:
        void launchStream(const std::string& url);
        void killStream();

        pid_t       mStreamPid{-1};
        std::string mCurrentUrl;
        bool        mIsPlaying{false};
    };

} // namespace Minivi
