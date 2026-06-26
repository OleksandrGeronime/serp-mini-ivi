#include "serp/serp.hpp"

#include "HmiQtRegistry.hpp"
#include "ICarRadioManager.hpp"
#include "ICarMediaManager.hpp"
#include "ICarClimateManager.hpp"
#include "ICarNavigationManager.hpp"
#include "ICarCallManager.hpp"
#include "ICarContactsManager.hpp"
#include "ICarPropertyService.hpp"
#include "ICarUserManager.hpp"
#include "ICarNotificationManager.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

// ── transport proxy declarations ─────────────────────────────────────────────
#if defined(MINI_HMI_REMOTE_GRPC)
#include "grpc/CarRadioManagerProxy.hpp"
#include "grpc/CarMediaManagerProxy.hpp"
#include "grpc/CarClimateManagerProxy.hpp"
#include "grpc/CarNavigationManagerProxy.hpp"
#include "grpc/CarCallManagerProxy.hpp"
#include "grpc/CarContactsManagerProxy.hpp"
#include "grpc/CarPropertyServiceProxy.hpp"
#include "grpc/CarUserManagerProxy.hpp"
#include "grpc/CarNotificationManagerProxy.hpp"
#else
#include "dbus/CarRadioManagerProxy.hpp"
#include "dbus/CarMediaManagerProxy.hpp"
#include "dbus/CarClimateManagerProxy.hpp"
#include "dbus/CarNavigationManagerProxy.hpp"
#include "dbus/CarCallManagerProxy.hpp"
#include "dbus/CarContactsManagerProxy.hpp"
#include "dbus/CarPropertyServiceProxy.hpp"
#include "dbus/CarUserManagerProxy.hpp"
#include "dbus/CarNotificationManagerProxy.hpp"
#endif

int main(int argc, char** argv) {
    QQuickStyle::setStyle("Basic");
    {
        const char* logDir = std::getenv("SERP_LOG_DIR");
        serp::FileLogConfig config;
        config.path = logDir ? logDir : "/tmp/serp/logs";
        config.fileNamePrefix = "hmi_qtquick6";
        config.maxFileSize = 10 * 1024 * 1024;
        config.maxRotatedFiles = 5;
        serp::logger::addStrategy(serp::createFileLogStrategy(config));
    }

    QGuiApplication app(argc, argv);

#if defined(MINI_HMI_REMOTE_GRPC)
    namespace ns = Minivi::serp_transport::grpc;
#else
    namespace ns = Minivi::serp_transport::dbus;
#endif

    auto iCarRadioManager        = ns::create_carradiomanager_icarradiomanager_proxy("hmi");
    auto iCarMediaManager        = ns::create_carmediamanager_icarmediamanager_proxy("hmi");
    auto iCarClimateManager      = ns::create_carclimatemanager_icarclimatemanager_proxy("hmi");
    auto iCarNavigationManager   = ns::create_carnavigationmanager_icarnavigationmanager_proxy("hmi");
    auto iCarCallManager         = ns::create_carcallmanager_icarcallmanager_proxy("hmi");
    auto iCarContactsManager     = ns::create_carcontactsmanager_icarcontactsmanager_proxy("hmi");
    auto iCarPropertyService     = ns::create_carpropertyservice_icarpropertyservice_proxy("hmi");
    auto iCarUserManager         = ns::create_carusermanager_icarusermanager_proxy("hmi");
    auto iCarNotificationManager = ns::create_carnotificationmanager_icarnotificationmanager_proxy("hmi");

    QQmlApplicationEngine engine;
    Minivi::HmiQtRegistry::init(engine, iCarRadioManager, iCarMediaManager,
                                       iCarClimateManager, iCarNavigationManager,
                                       iCarCallManager, iCarContactsManager,
                                       iCarPropertyService,
                                       iCarUserManager, iCarNotificationManager);

    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) return 1;

    const int result = app.exec();
    serp::App::stopAll();
    return result;
}
