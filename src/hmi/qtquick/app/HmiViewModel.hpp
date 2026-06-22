#pragma once

#include "HmiTypes.hpp"
#include "RemoteHmiClient.hpp"

#include <QObject>
#include <QTimer>
#include <QVariantList>

#include <memory>

namespace mini_hmi_qt {

class HmiViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString screen READ screen NOTIFY stateChanged)
    Q_PROPERTY(QString seat READ seat NOTIFY stateChanged)
    Q_PROPERTY(QString lastAction READ lastAction NOTIFY stateChanged)
    Q_PROPERTY(QString message READ message NOTIFY stateChanged)
    Q_PROPERTY(QString mediaState READ mediaState NOTIFY stateChanged)
    Q_PROPERTY(QString mediaSource READ mediaSource NOTIFY stateChanged)
    Q_PROPERTY(QString mediaTrack READ mediaTrack NOTIFY stateChanged)
    Q_PROPERTY(QString mediaArtist READ mediaArtist NOTIFY stateChanged)
    Q_PROPERTY(QString mediaTrackIndex READ mediaTrackIndex NOTIFY stateChanged)
    Q_PROPERTY(QString mediaPlaylistSize READ mediaPlaylistSize NOTIFY stateChanged)
    Q_PROPERTY(QString audioFocus READ audioFocus NOTIFY stateChanged)
    Q_PROPERTY(QString radioStation READ radioStation NOTIFY stateChanged)
    Q_PROPERTY(QString radioBand READ radioBand NOTIFY stateChanged)
    Q_PROPERTY(QString radioFreq READ radioFreq NOTIFY stateChanged)
    Q_PROPERTY(QString radioName READ radioName NOTIFY stateChanged)
    Q_PROPERTY(QString radioStatus READ radioStatus NOTIFY stateChanged)
    Q_PROPERTY(int radioSignal READ radioSignal NOTIFY stateChanged)
    Q_PROPERTY(QString navigationState READ navigationState NOTIFY stateChanged)
    Q_PROPERTY(QString navigationDestination READ navigationDestination NOTIFY stateChanged)
    Q_PROPERTY(QString phoneState READ phoneState NOTIFY stateChanged)
    Q_PROPERTY(QString phoneNumber READ phoneNumber NOTIFY stateChanged)
    Q_PROPERTY(QString climateDriver READ climateDriver NOTIFY stateChanged)
    Q_PROPERTY(QString climatePassenger READ climatePassenger NOTIFY stateChanged)
    Q_PROPERTY(QString climateFan READ climateFan NOTIFY stateChanged)
    Q_PROPERTY(QString profile READ profile NOTIFY stateChanged)
    Q_PROPERTY(QString vehicleLine READ vehicleLine NOTIFY stateChanged)
    Q_PROPERTY(QString settingsLine READ settingsLine NOTIFY stateChanged)
    Q_PROPERTY(QString notifications READ notifications NOTIFY stateChanged)
    Q_PROPERTY(QString navLatitude  READ navLatitude  NOTIFY stateChanged)
    Q_PROPERTY(QString navLongitude READ navLongitude NOTIFY stateChanged)
    Q_PROPERTY(QString navSpeed     READ navSpeed     NOTIFY stateChanged)
    Q_PROPERTY(QString navHeading   READ navHeading   NOTIFY stateChanged)
    Q_PROPERTY(QVariantList navItems READ navItems NOTIFY stateChanged)
    Q_PROPERTY(QVariantList actions READ actions NOTIFY stateChanged)

public:
    explicit HmiViewModel(QObject* parent = nullptr);
    ~HmiViewModel() override;

    QString screen() const;
    QString seat() const;
    QString lastAction() const;
    QString message() const;
    QString mediaState() const;
    QString mediaSource() const;
    QString mediaTrack() const;
    QString mediaArtist() const;
    QString mediaTrackIndex() const;
    QString mediaPlaylistSize() const;
    QString audioFocus() const;
    QString radioStation() const;
    QString radioBand() const;
    QString radioFreq() const;
    QString radioName() const;
    QString radioStatus() const;
    int radioSignal() const;
    QString navigationState() const;
    QString navigationDestination() const;
    QString phoneState() const;
    QString phoneNumber() const;
    QString climateDriver() const;
    QString climatePassenger() const;
    QString climateFan() const;
    QString profile() const;
    QString vehicleLine() const;
    QString settingsLine() const;
    QString notifications() const;
    QString navLatitude() const;
    QString navLongitude() const;
    QString navSpeed() const;
    QString navHeading() const;
    QVariantList navItems() const;
    QVariantList actions() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void openScreen(const QString& id);
    Q_INVOKABLE void toggleSeat();
    Q_INVOKABLE void triggerAction(int index);

signals:
    void stateChanged();

private:
    void rebuild();
    QString currentUser() const;

    mini_hmi::RemoteHmiClient mClient;
    mini_hmi::ViewModel mView;
    bool mStarted{false};
    QTimer mGpsTimer;
};

} // namespace mini_hmi_qt
