// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "LocationHal.hpp"

#include <algorithm>
#include <cmath>
#include <mutex>
#include <sstream>

namespace Minivi
{

namespace {

struct Waypoint { double lat; double lon; };

static constexpr Waypoint kRoute[] = {
    {42.3314, -83.0458}, {42.3370, -83.0502}, {42.3500, -83.0590},
    {42.3620, -83.0700}, {42.3700, -83.0600}, {42.3650, -83.0450},
    {42.3550, -83.0350}, {42.3400, -83.0400}, {42.3314, -83.0458}
};
static constexpr int    kRouteSize       = 9;
static constexpr double kSpeedMps        = 30.0;
static constexpr double kMetersPerDegLat = 111320.0;
static const     double kPi              = std::acos(-1.0);

GpsPosition computePosition(double elapsed)
{
    double segLens[kRouteSize - 1];
    double totalLen = 0.0;
    for (int i = 0; i < kRouteSize - 1; ++i) {
        double dlat = (kRoute[i+1].lat - kRoute[i].lat) * kMetersPerDegLat;
        double avgLat = (kRoute[i].lat + kRoute[i+1].lat) * 0.5;
        double mPerDegLon = kMetersPerDegLat * std::cos(avgLat * kPi / 180.0);
        double dlon = (kRoute[i+1].lon - kRoute[i].lon) * mPerDegLon;
        segLens[i] = std::sqrt(dlat * dlat + dlon * dlon);
        totalLen += segLens[i];
    }

    double dist = std::fmod(elapsed * kSpeedMps, totalLen);
    double lat = kRoute[0].lat, lon = kRoute[0].lon, heading = 0.0;
    double accumulated = 0.0;
    for (int i = 0; i < kRouteSize - 1; ++i) {
        if (dist < accumulated + segLens[i]) {
            double t = (dist - accumulated) / segLens[i];
            lat = kRoute[i].lat + t * (kRoute[i+1].lat - kRoute[i].lat);
            lon = kRoute[i].lon + t * (kRoute[i+1].lon - kRoute[i].lon);
            double dlat = (kRoute[i+1].lat - kRoute[i].lat) * kMetersPerDegLat;
            double avgLat2 = (kRoute[i].lat + kRoute[i+1].lat) * 0.5;
            double mPerDegLon = kMetersPerDegLat * std::cos(avgLat2 * kPi / 180.0);
            double dlon = (kRoute[i+1].lon - kRoute[i].lon) * mPerDegLon;
            heading = std::atan2(dlon, dlat) * 180.0 / kPi;
            if (heading < 0.0) heading += 360.0;
            break;
        }
        accumulated += segLens[i];
    }
    return GpsPosition{lat, lon, kSpeedMps, heading};
}

} // namespace

    std::shared_ptr<ILocationHal> ILocationHal::create() {
        return std::make_shared<LocationHal>();
    }

    LocationHal::LocationHal()
        : LocationHalBase()
    {
        logMethod("LocationHal");
        PositionStream.setHandler([this](serp::StreamWriter<GpsPosition> writer, const int32_t& minAccuracyM) {
            {
                std::lock_guard<std::mutex> lock(mWritersMutex);
                mStreamWriters.push_back(writer);
                logInfo() << "PositionStream subscriber added, total=" << mStreamWriters.size() << " minAcc=" << minAccuracyM;
            }
            writer->onCancel([this, writer]() {
                std::lock_guard<std::mutex> lock(mWritersMutex);
                mStreamWriters.erase(
                    std::remove_if(mStreamWriters.begin(), mStreamWriters.end(),
                        [&writer](const serp::StreamWriter<GpsPosition>& w) { return w.get() == writer.get(); }),
                    mStreamWriters.end());
                logInfo() << "PositionStream subscriber removed, total=" << mStreamWriters.size();
            });
        });
    }

    LocationHal::~LocationHal() noexcept = default;

    void LocationHal::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit — starting GPS simulation (Detroit route)";
        mStartTime = std::chrono::steady_clock::now();
        mPositionTimer = serp::Timer::create(std::chrono::milliseconds{500}, true, [this](serp::TimerPtr) {
            onPositionTick();
        });
        mPositionTimer->start();
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void LocationHal::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        if (mPositionTimer) { mPositionTimer->stop(); mPositionTimer.reset(); }
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void LocationHal::onPositionTick()
    {
        const double elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - mStartTime).count();
        const GpsPosition pos = computePosition(elapsed);
        CurrentPosition = pos;
        PositionUpdated(pos);
        std::lock_guard<std::mutex> lock(mWritersMutex);
        for (auto& writer : mStreamWriters) {
            writer->write(pos);
        }
    }

    void LocationHal::calculateRoute(serp::ResponsePtr<std::string> reply, const std::string& destination)
    {
        logInfo() << "calculateRoute destination=" << destination;
        ActiveDestination = destination;
        RouteCalculated(destination, "direct");
        reply->call("direct");
    }

    void LocationHal::nextManeuver(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentManeuver));
    }

    void LocationHal::frame(serp::ResponsePtr<std::string> reply)
    {
        const GpsPosition pos = static_cast<GpsPosition>(CurrentPosition);
        std::ostringstream out;
        out << "location.lat="     << pos.latitude  << "\n";
        out << "location.lon="     << pos.longitude << "\n";
        out << "location.speed="   << pos.speed     << "\n";
        out << "location.heading=" << pos.heading   << "\n";
        out << "location.dest="    << static_cast<std::string>(ActiveDestination) << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
