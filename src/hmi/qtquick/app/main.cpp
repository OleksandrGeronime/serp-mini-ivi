#include "serp/serp.hpp"

#include "HmiViewModel.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

int main(int argc, char** argv) {
    {
        const char* logDir = std::getenv("SERP_LOG_DIR");
        serp::FileLogConfig config;
        config.path = logDir ? logDir : "/tmp/serp/logs";
        config.fileNamePrefix = "hmi_qtquick_client";
        config.maxFileSize = 10 * 1024 * 1024;
        config.maxRotatedFiles = 5;
        serp::logger::addStrategy(serp::createFileLogStrategy(config));
    }

    QGuiApplication app(argc, argv);

    mini_hmi_qt::HmiViewModel viewModel;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("hmi", &viewModel);
    engine.load(QUrl(QStringLiteral("qrc:/qml/Main.qml")));
    if (engine.rootObjects().isEmpty()) return 1;

    viewModel.start();
    return app.exec();
}
