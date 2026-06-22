#include "HmiApp.hpp"
#include "HmiController.hpp"
#if defined(MINI_HMI_REMOTE_DBUS) || defined(MINI_HMI_REMOTE_GRPC)
#include "RemoteHmiClient.hpp"
#else
#include "Minivi.hpp"
#endif

#include "serp/serp.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // The infotainment HMI owns the terminal. Keep SERP logs off stdout so the
    // button UI stays clean.
    serp::logger::clearStrategies();

    bool scripted = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--scripted" || std::string(argv[i]) == "--script") scripted = true;
    }

#if defined(MINI_HMI_REMOTE_DBUS) || defined(MINI_HMI_REMOTE_GRPC)
    mini_hmi::RemoteHmiClient platform;
#else
    mini_hmi::Minivi platform;
#endif
    try {
        platform.start();
        mini_hmi::HmiController controller(platform);
        mini_hmi::HmiApp app(controller);
        const int result = scripted ? app.runScripted() : app.runInteractive();
        platform.stop();
        return result;
    } catch (const std::exception& ex) {
        platform.stop();
        std::cerr << "Mini platform HMI failed: " << ex.what() << "\n";
        return 1;
    }
}
