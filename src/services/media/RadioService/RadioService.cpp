// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "RadioService.hpp"

#include "IAudioFocusService.hpp"
#include "IRadioHal.hpp"
#include "IPersistenceService.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

namespace Minivi
{

namespace {

// Keys match the frequency strings the HMI sends (87.5 = MHz, AM:612 = AM kHz)
static const std::vector<std::string> kPresets = {
    "87.5", "98.7", "101.1", "103.7", "107.9", "AM:612"
};

static const std::string kDefaultFm = "87.5";
static const std::string kDefaultAm = "AM:612";

} // namespace

    std::shared_ptr<IRadioService> IRadioService::create(std::shared_ptr<IAudioFocusService> audiofocusservice, std::shared_ptr<IRadioHal> radiohal, std::shared_ptr<IPersistenceService> persistenceservice) {
        return std::make_shared<RadioService>(audiofocusservice, radiohal, persistenceservice);
    }

    RadioService::RadioService(std::shared_ptr<IAudioFocusService> audiofocusservice, std::shared_ptr<IRadioHal> radiohal, std::shared_ptr<IPersistenceService> persistenceservice)
        : RadioServiceBase(audiofocusservice, radiohal, persistenceservice)
    {
        logMethod("RadioService");
        CurrentTunerStation.assignFromTransport(kDefaultFm);
    }

    RadioService::~RadioService() noexcept = default;

    void RadioService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        CurrentTunerStation = static_cast<std::string>(CurrentTunerStation);
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void RadioService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void RadioService::tune(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& frequency)
    {
        logInfo() << "tune user=" << user << " freq=" << frequency;

        if (frequency == "stop") {
            mRadioHal->Tune("stop");
            CurrentTunerStation = "";
            TunerChanged("", "stopped");
            reply->call(true);
            return;
        }
        if (frequency == "FM") {
            const std::string& station = kDefaultFm;
            CurrentTunerStation = station;
            TunerChanged(station, "band_fm");
            mRadioHal->Tune(station);
            reply->call(true);
            return;
        }
        if (frequency == "AM") {
            const std::string& station = kDefaultAm;
            CurrentTunerStation = station;
            TunerChanged(station, "band_am");
            mRadioHal->Tune(station);
            reply->call(true);
            return;
        }

        CurrentTunerStation = frequency;
        TunerChanged(frequency, "tuned");
        mRadioHal->Tune(frequency);
        reply->call(true);
    }

    void RadioService::seek(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& direction)
    {
        logInfo() << "seek user=" << user << " dir=" << direction;
        const std::string current = static_cast<std::string>(CurrentTunerStation);
        const auto it = std::find(kPresets.begin(), kPresets.end(), current);
        std::string next;
        if (it == kPresets.end()) {
            next = kPresets.front();
        } else if (direction == "up") {
            const auto nit = std::next(it);
            next = (nit == kPresets.end()) ? kPresets.front() : *nit;
        } else {
            next = (it == kPresets.begin()) ? kPresets.back() : *std::prev(it);
        }
        CurrentTunerStation = next;
        TunerChanged(next, "seek");
        mRadioHal->Tune(next);
        reply->call(true);
    }

    void RadioService::tunerStation(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentTunerStation));
    }

    void RadioService::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        out << "radio.station=" << static_cast<std::string>(CurrentTunerStation) << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
