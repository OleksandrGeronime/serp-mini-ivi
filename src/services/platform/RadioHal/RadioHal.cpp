// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "RadioHal.hpp"

#include <algorithm>
#include <csignal>
#include <fcntl.h>
#include <map>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace Minivi
{

namespace {

// SomaFM stream URLs — keys match the frequency strings sent by HMI buttons.
// SomaFM has retired most 256 kbps MP3 endpoints; use 128 kbps which are reliably available.
// ice2/ice4 mirrors used for stations that only have 404 on ice1 at 128 kbps.
static const std::map<std::string, std::pair<std::string, std::string>> kStations = {
    // key → {display_name, stream_url}
    {"87.5",   {"Groove Salad",    "http://ice1.somafm.com/groovesalad-128-mp3"}},
    {"98.7",   {"Secret Agent",    "http://ice2.somafm.com/secretagent-128-mp3"}},
    {"101.1",  {"Indie Pop Rocks", "http://ice1.somafm.com/indiepop-128-mp3"}},
    {"103.7",  {"Beat Blender",    "http://ice1.somafm.com/beatblender-128-mp3"}},
    {"107.9",  {"Mission Control", "http://ice1.somafm.com/missioncontrol-128-mp3"}},
    {"AM:612", {"The Trip",        "http://ice1.somafm.com/thetrip-128-mp3"}},
};

// Ordered list of station keys for seek
static const std::vector<std::string> kStationOrder = {
    "87.5","98.7","101.1","103.7","107.9","AM:612"
};

} // namespace

    std::shared_ptr<IRadioHal> IRadioHal::create() {
        return std::make_shared<RadioHal>();
    }

    RadioHal::RadioHal()
        : RadioHalBase()
    {
        logMethod("RadioHal");
        CurrentRadioStation.assignFromTransport("87.5");
    }

    RadioHal::~RadioHal() noexcept
    {
        killStream();
    }

    void RadioHal::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        CurrentRadioStation = static_cast<std::string>(CurrentRadioStation);
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void RadioHal::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        killStream();
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void RadioHal::tune(serp::ResponsePtr<std::string> reply, const std::string& frequency)
    {
        logInfo() << "tune frequency=" << frequency;
        if (frequency == "stop") {
            killStream();
            CurrentRadioStation = "";
            reply->call("stopped");
            return;
        }
        const auto it = kStations.find(frequency);
        if (it == kStations.end()) {
            logWarn() << "RadioHal::tune unknown frequency: " << frequency;
            reply->call(frequency);
            return;
        }
        const auto& [name, url] = it->second;
        mCurrentUrl = url;
        CurrentRadioStation = frequency;
        launchStream(url);
        RadioHardwareTuned(frequency);
        reply->call(name);  // station name propagated up to CarRadioManager
    }

    void RadioHal::seek(serp::ResponsePtr<std::string> reply, const SeekDirection& direction)
    {
        logInfo() << "seek direction=" << static_cast<int>(direction);
        const std::string current = static_cast<std::string>(CurrentRadioStation);
        const auto it = std::find(kStationOrder.begin(), kStationOrder.end(), current);
        std::string next;
        if (it == kStationOrder.end()) {
            next = kStationOrder.front();
        } else if (direction == SeekDirection::up) {
            const auto nit = std::next(it);
            next = (nit == kStationOrder.end()) ? kStationOrder.front() : *nit;
        } else {
            next = (it == kStationOrder.begin()) ? kStationOrder.back() : *std::prev(it);
        }
        const auto& [name, url] = kStations.at(next);
        mCurrentUrl = url;
        CurrentRadioStation = next;
        launchStream(url);
        RadioHardwareTuned(next);
        reply->call(name);
    }

    void RadioHal::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        const std::string freq = static_cast<std::string>(CurrentRadioStation);
        out << "radiohal.freq="    << freq       << "\n";
        out << "radiohal.playing=" << mIsPlaying << "\n";
        const auto it = kStations.find(freq);
        if (it != kStations.end())
            out << "radiohal.name=" << it->second.first << "\n";
        reply->call(out.str());
    }

    static constexpr const char* kRadioPidFile = "/tmp/serp_radio_pgid";

    void RadioHal::launchStream(const std::string& url)
    {
        killStream();
        logInfo() << "launchStream url=" << url;
        const pid_t pid = fork();
        if (pid < 0) {
            logError() << "RadioHal::launchStream fork failed";
            mIsPlaying = false;
            return;
        }
        if (pid == 0) {
            // New process group so killStream() can kill this process AND its afplay children
            setpgid(0, 0);
            const int fd = open("/dev/null", O_WRONLY);
            if (fd >= 0) { dup2(fd, STDOUT_FILENO); dup2(fd, STDERR_FILENO); close(fd); }
#ifdef __APPLE__
            // afplay costs ~0.82 s startup+teardown per invocation.
            // Fix: downloader thread fills a queue; player pre-starts next afplay
            // OVERLAP seconds before current audio ends — old process reaped in a
            // daemon thread so it never blocks the main loop.
            static const char* kScript =
                "import sys,os,urllib.request,subprocess,signal,threading,queue,time\n"
                "os.setpgrp()\n"
                "url=sys.argv[1]\n"
                "CHUNK=65536\n"
                "BITRATE=128000.0\n"
                "OVERLAP=0.4\n"
                "files=['/tmp/serpradio_a.mp3','/tmp/serpradio_b.mp3']\n"
                "Q=queue.Queue(maxsize=3)\n"
                "stop=threading.Event()\n"
                "ref=[None]\n"
                "def sig(s,f):\n"
                " stop.set()\n"
                " p=ref[0]\n"
                " if p:\n"
                "  try:p.terminate()\n"
                "  except:pass\n"
                " sys.exit(0)\n"
                "signal.signal(signal.SIGTERM,sig)\n"
                "def dl():\n"
                " req=urllib.request.Request(url,headers={'User-Agent':'Mozilla/5.0','Icy-MetaData':'0'})\n"
                " try:resp=urllib.request.urlopen(req,timeout=15)\n"
                " except:Q.put(None);return\n"
                " i=0\n"
                " while not stop.is_set():\n"
                "  try:data=resp.read(CHUNK)\n"
                "  except:break\n"
                "  if not data:break\n"
                "  fn=files[i%2]\n"
                "  with open(fn,'wb') as f:f.write(data)\n"
                "  try:Q.put(fn,timeout=10)\n"
                "  except:break\n"
                "  i+=1\n"
                " try:Q.put(None,timeout=5)\n"
                " except:pass\n"
                "threading.Thread(target=dl,daemon=True).start()\n"
                "cur=None\n"
                "while not stop.is_set():\n"
                " try:fn=Q.get(timeout=30)\n"
                " except queue.Empty:break\n"
                " if fn is None:break\n"
                " asec=os.path.getsize(fn)*8.0/BITRATE\n"
                " p=subprocess.Popen(['afplay',fn])\n"
                " ref[0]=p\n"
                " old=cur\n"
                " if old:threading.Thread(target=old.wait,daemon=True).start()\n"
                " cur=p\n"
                " time.sleep(max(0.0,asec-OVERLAP))\n"
                "if cur:cur.wait()\n";
            execlp("python3", "python3", "-c", kScript, url.c_str(), nullptr);
#else
            execlp("ffplay", "ffplay", "-nodisp", "-loglevel", "quiet",
                   "-infbuf", url.c_str(), nullptr);
#endif
            _exit(1);
        }
        // Set child's process group from parent side too (avoids race with child's setpgid)
        setpgid(pid, pid);
        mStreamPid = pid;
        mIsPlaying = true;
        // Write PGID to file so stop.sh can kill the group even if our destructor never runs
        if (FILE* f = fopen(kRadioPidFile, "w")) {
            fprintf(f, "%d\n", pid);
            fclose(f);
        }
    }

    void RadioHal::killStream()
    {
        if (mStreamPid > 0) {
            // Kill entire process group: Python3 + all spawned afplay children
            kill(-mStreamPid, SIGTERM);
            waitpid(mStreamPid, nullptr, WNOHANG);
            mStreamPid = -1;
        }
        mIsPlaying = false;
        remove(kRadioPidFile);
    }

} // namespace Minivi
