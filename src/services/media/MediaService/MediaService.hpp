// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <sys/types.h>
#include <tuple>
#include <vector>

#include "MediaServiceBase.hpp"

namespace Minivi
{
    class MediaService : public MediaServiceBase
    {
    public:
        explicit MediaService(std::shared_ptr<IAudioFocusService> audiofocusservice);
        ~MediaService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void play(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& source) override;
        void pause(serp::ResponsePtr<bool> reply, const std::string& reason) override;
        void next(serp::ResponsePtr<bool> reply, const std::string& user) override;
        void onFocusEvent(serp::ResponsePtr<bool> reply, const std::string& event) override;
        void recent(serp::ResponsePtr<std::vector<TrackInfo>> reply) override;
        void sourceHistory(serp::ResponsePtr<std::map<std::string, TrackInfo>> reply) override;
        void getPlaybackState(serp::ResponsePtr<PlaybackState> reply) override;
        void playbackSource(serp::ResponsePtr<std::string> reply) override;
        void playbackTrack(serp::ResponsePtr<std::optional<TrackInfo>> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;

    private:
        void scanPlaylist(const std::string& dir);
        void playTrack(int index);
        void stopPlayback();
        void onPlaybackTick();
        TrackInfo probeTrack(const std::string& path) const;

        std::vector<std::string>         mPlaylist;
        int                              mCurrentIndex{0};
        pid_t                            mPlayPid{-1};
        serp::TimerPtr                   mPlayTimer;
        std::map<std::string, TrackInfo> mTrackCache;
    };

} // namespace Minivi
