// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "MediaService.hpp"

#include "IAudioFocusService.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <dirent.h>
#include <fcntl.h>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace Minivi
{

namespace {

static const std::vector<std::string> kAudioExts = {".mp3", ".flac", ".ogg", ".oga", ".wav", ".m4a", ".aac", ".aiff", ".aif"};

// Bundled demo tracks — populated by the asset pipeline (assets/media/)
static const std::map<std::string, std::pair<std::string, std::string>> kBundledTrackMeta = {
    {"track_01", {"Groove Salad",      "SomaFM / Demo"}},
    {"track_02", {"Secret Agent",      "SomaFM / Demo"}},
    {"track_03", {"Indie Pop Rocks",   "SomaFM / Demo"}},
    {"track_04", {"Beat Blender",      "SomaFM / Demo"}},
    {"track_05", {"Mission Control",   "SomaFM / Demo"}},
};
std::vector<std::string> buildSearchDirs()
{
    std::vector<std::string> dirs;
    if (const char* root = std::getenv("SERP_PRODUCT_ROOT")) {
        dirs.push_back(std::string(root) + "/assets/media");
    }
#ifdef __APPLE__
    dirs.push_back("/System/Library/Sounds");
    dirs.push_back("/tmp/music");
#else
    dirs.insert(dirs.end(), {"/home/alex/Music", "/home/alex/samples", "/root/Music", "/tmp/music",
                              "/usr/share/sounds/alsa", "/usr/share/sounds/freedesktop/stereo",
                              "/usr/share/sounds/gnome/default/alerts"});
#endif
    return dirs;
}

bool hasAudioExt(const std::string& name)
{
    for (const auto& ext : kAudioExts)
        if (name.size() >= ext.size() &&
            name.compare(name.size() - ext.size(), ext.size(), ext) == 0)
            return true;
    return false;
}

std::string baseName(const std::string& path)
{
    const auto pos = path.rfind('/');
    return pos == std::string::npos ? path : path.substr(pos + 1);
}

std::string stripExt(const std::string& name)
{
    const auto pos = name.rfind('.');
    return pos == std::string::npos ? name : name.substr(0, pos);
}

} // namespace

    std::shared_ptr<IMediaService> IMediaService::create(std::shared_ptr<IAudioFocusService> audiofocusservice) {
        return std::make_shared<MediaService>(audiofocusservice);
    }

    MediaService::MediaService(std::shared_ptr<IAudioFocusService> audiofocusservice)
        : MediaServiceBase(audiofocusservice)
    {
        logMethod("MediaService");
        CurrentPlaybackState.assignFromTransport(PlaybackState::stopped);
    }

    MediaService::~MediaService() noexcept
    {
        stopPlayback();
    }

    void MediaService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        CurrentPlaybackState = static_cast<PlaybackState>(CurrentPlaybackState);

        for (const auto& dir : buildSearchDirs())
            scanPlaylist(dir);
        logInfo() << "found " << mPlaylist.size() << " tracks";
        CurrentPlaylistSize = static_cast<uint32_t>(mPlaylist.size());
        CurrentTrackIndex   = mCurrentIndex;

        reply(serp::Service::Status::SUCCESSFUL);
    }

    void MediaService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        stopPlayback();
        if (mPlayTimer) { mPlayTimer->stop(); mPlayTimer.reset(); }
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void MediaService::scanPlaylist(const std::string& dir)
    {
        DIR* d = opendir(dir.c_str());
        if (!d) return;
        struct dirent* ent;
        while ((ent = readdir(d)) != nullptr) {
            const std::string name(ent->d_name);
            if (name.empty() || name[0] == '.') continue;
            const std::string path = dir + "/" + name;
            struct stat st;
            if (stat(path.c_str(), &st) != 0) continue;
            if (S_ISREG(st.st_mode) && hasAudioExt(name))
                mPlaylist.push_back(path);
        }
        closedir(d);
        std::sort(mPlaylist.begin(), mPlaylist.end());
    }

    TrackInfo MediaService::probeTrack(const std::string& path) const
    {
        const auto it = mTrackCache.find(path);
        if (it != mTrackCache.end()) return it->second;

        TrackInfo info{};
        const std::string stem = stripExt(baseName(path));
        const auto meta = kBundledTrackMeta.find(stem);
        if (meta != kBundledTrackMeta.end()) {
            info.title  = meta->second.first;
            info.artist = meta->second.second;
        } else {
            info.title  = stem;
            info.artist = "Unknown";
        }
        info.durationMs = 0;

#ifndef __APPLE__
        std::array<char, 256> buf;
        const std::string cmd = "ffprobe -v error -show_entries format=duration"
                                " -of default=noprint_wrappers=1:nokey=1 \"" + path + "\" 2>/dev/null";
        FILE* fp = popen(cmd.c_str(), "r");
        if (fp) {
            if (fgets(buf.data(), buf.size(), fp))
                info.durationMs = static_cast<uint32_t>(std::stof(buf.data()) * 1000.0f);
            pclose(fp);
        }
#endif
        return info;
    }

    void MediaService::playTrack(int index)
    {
        stopPlayback();
        if (mPlaylist.empty()) {
            logWarn() << "MediaService::playTrack no tracks in playlist";
            CurrentPlaybackState = PlaybackState::stopped;
            PlaybackChanged(PlaybackState::stopped, std::nullopt);
            return;
        }
        mCurrentIndex = ((index % static_cast<int>(mPlaylist.size())) + static_cast<int>(mPlaylist.size())) % static_cast<int>(mPlaylist.size());
        CurrentTrackIndex = mCurrentIndex;
        const std::string& path = mPlaylist[mCurrentIndex];

        const TrackInfo info = probeTrack(path);
        const_cast<std::map<std::string, TrackInfo>&>(mTrackCache)[path] = info;

        logInfo() << "playTrack [" << mCurrentIndex << "] " << info.title;

        const pid_t pid = fork();
        if (pid < 0) {
            logError() << "MediaService::playTrack fork failed";
            CurrentPlaybackState = PlaybackState::stopped;
            PlaybackChanged(PlaybackState::stopped, std::nullopt);
            return;
        }
        if (pid == 0) {
            const int fd = open("/dev/null", O_WRONLY);
            if (fd >= 0) { dup2(fd, STDOUT_FILENO); dup2(fd, STDERR_FILENO); close(fd); }
#ifdef __APPLE__
            execlp("afplay", "afplay", path.c_str(), nullptr);
#else
            execlp("ffplay", "ffplay", "-nodisp", "-loglevel", "quiet",
                   "-autoexit", path.c_str(), nullptr);
#endif
            _exit(1);
        }
        mPlayPid = pid;
        CurrentPlaybackState  = PlaybackState::playing;
        CurrentPlaybackTrack  = info;
        PlaybackChanged(PlaybackState::playing, info);

        // Poll to detect track end and auto-advance
        mPlayTimer = serp::Timer::create(std::chrono::milliseconds{1000}, true, [this](serp::TimerPtr) {
            onPlaybackTick();
        });
        mPlayTimer->start();
    }

    void MediaService::stopPlayback()
    {
        if (mPlayTimer) { mPlayTimer->stop(); mPlayTimer.reset(); }
        if (mPlayPid > 0) {
            kill(mPlayPid, SIGTERM);
            waitpid(mPlayPid, nullptr, WNOHANG);
            mPlayPid = -1;
        }
    }

    void MediaService::onPlaybackTick()
    {
        if (mPlayPid <= 0) return;
        if (static_cast<PlaybackState>(CurrentPlaybackState) == PlaybackState::paused) return;
        int status = 0;
        const pid_t result = waitpid(mPlayPid, &status, WNOHANG);
        if (result == mPlayPid) {
            // Track ended — auto-advance
            mPlayPid = -1;
            if (mPlayTimer) { mPlayTimer->stop(); mPlayTimer.reset(); }
            logInfo() << "track ended, advancing";
            playTrack(mCurrentIndex + 1);
        }
    }

    void MediaService::play(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& source)
    {
        logInfo() << "play user=" << user << " source=" << source;
        if (!source.empty())
            CurrentPlaybackSource = source;

        if (static_cast<PlaybackState>(CurrentPlaybackState) == PlaybackState::paused && mPlayPid > 0) {
            // Resume by sending SIGCONT
            kill(mPlayPid, SIGCONT);
            CurrentPlaybackState = PlaybackState::playing;
            PlaybackChanged(PlaybackState::playing, static_cast<std::optional<TrackInfo>>(CurrentPlaybackTrack));
        } else {
            playTrack(mCurrentIndex);
        }
        reply->call(true);
    }

    void MediaService::pause(serp::ResponsePtr<bool> reply, const std::string& reason)
    {
        logInfo() << "pause reason=" << reason;
        if (mPlayPid > 0) {
            kill(mPlayPid, SIGSTOP);
        }
        CurrentPlaybackState = PlaybackState::paused;
        PlaybackChanged(PlaybackState::paused, static_cast<std::optional<TrackInfo>>(CurrentPlaybackTrack));
        reply->call(true);
    }

    void MediaService::next(serp::ResponsePtr<bool> reply, const std::string& user)
    {
        logInfo() << "next user=" << user;
        playTrack(mCurrentIndex + 1);
        reply->call(true);
    }

    void MediaService::onFocusEvent(serp::ResponsePtr<bool> reply, const std::string& event)
    {
        logInfo() << "onFocusEvent event=" << event;
        if (event == "lost") {
            if (static_cast<PlaybackState>(CurrentPlaybackState) == PlaybackState::playing) {
                if (mPlayPid > 0) kill(mPlayPid, SIGSTOP);
                CurrentPlaybackState = PlaybackState::paused;
                PlaybackChanged(PlaybackState::paused, static_cast<std::optional<TrackInfo>>(CurrentPlaybackTrack));
            }
        } else if (event == "gained") {
            if (static_cast<PlaybackState>(CurrentPlaybackState) == PlaybackState::paused && mPlayPid > 0) {
                kill(mPlayPid, SIGCONT);
                CurrentPlaybackState = PlaybackState::playing;
                PlaybackChanged(PlaybackState::playing, static_cast<std::optional<TrackInfo>>(CurrentPlaybackTrack));
            }
        }
        reply->call(true);
    }

    void MediaService::recent(serp::ResponsePtr<std::vector<TrackInfo>> reply)
    {
        std::vector<TrackInfo> result;
        result.reserve(mTrackCache.size());
        for (const auto& [_, info] : mTrackCache)
            result.push_back(info);
        reply->call(result);
    }

    void MediaService::sourceHistory(serp::ResponsePtr<std::map<std::string, TrackInfo>> reply)
    {
        reply->call(mTrackCache);
    }

    void MediaService::getPlaybackState(serp::ResponsePtr<PlaybackState> reply)
    {
        reply->call(static_cast<PlaybackState>(CurrentPlaybackState));
    }

    void MediaService::playbackSource(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentPlaybackSource));
    }

    void MediaService::playbackTrack(serp::ResponsePtr<std::optional<TrackInfo>> reply)
    {
        reply->call(static_cast<std::optional<TrackInfo>>(CurrentPlaybackTrack));
    }

    void MediaService::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        std::ostringstream ss;
        ss << static_cast<PlaybackState>(CurrentPlaybackState);
        out << "media.state="   << ss.str() << "\n";
        out << "media.source="  << static_cast<std::string>(CurrentPlaybackSource) << "\n";
        const auto track = static_cast<std::optional<TrackInfo>>(CurrentPlaybackTrack);
        if (track) {
            out << "media.track="  << track->title  << "\n";
            out << "media.artist=" << track->artist << "\n";
        }
        out << "media.playlist_size=" << mPlaylist.size() << "\n";
        out << "media.track_index="   << mCurrentIndex    << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
