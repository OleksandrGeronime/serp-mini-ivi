cmake_minimum_required(VERSION 3.16)
project(minivi_qtquick_hmi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Location Positioning)

set(MINI_HMI_DIR ${CMAKE_CURRENT_LIST_DIR}/../console)

add_executable(minivi_qtquick_hmi
    app/main.cpp
    app/HmiViewModel.cpp
    qml/qml.qrc
    ${MINI_HMI_DIR}/client/RemoteHmiClient.cpp
    ${MINI_HMI_DIR}/modules/HmiModules.cpp
    ${MINI_HMI_DIR}/modules/HomeModule.cpp
    ${MINI_HMI_DIR}/modules/MediaPlayerModule.cpp
    ${MINI_HMI_DIR}/modules/RadioModule.cpp
    ${MINI_HMI_DIR}/modules/NavigationModule.cpp
    ${MINI_HMI_DIR}/modules/PhoneModule.cpp
    ${MINI_HMI_DIR}/modules/ClimateModule.cpp
    ${MINI_HMI_DIR}/modules/SettingsModule.cpp
    ${MINI_HMI_DIR}/modules/ProfileModule.cpp
    ${MINI_HMI_DIR}/modules/VehicleModule.cpp
    ${MINI_HMI_DIR}/modules/NotificationsModule.cpp
    ${MINI_HMI_DIR}/state/HmiTypes.cpp
    ${MINI_HMI_DIR}/ui/UiPrimitives.cpp
)

target_include_directories(minivi_qtquick_hmi PRIVATE
    app
    ${MINI_HMI_DIR}/client
    ${MINI_HMI_DIR}/modules
    ${MINI_HMI_DIR}/state
    ${MINI_HMI_DIR}/ui
)

target_compile_definitions(minivi_qtquick_hmi PRIVATE LOG_LEVEL=5)

target_link_libraries(minivi_qtquick_hmi PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Location
    Qt5::Positioning
    serp
)
