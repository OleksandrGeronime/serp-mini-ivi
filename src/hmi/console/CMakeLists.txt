add_executable(minivi_console_hmi
    shell/main.cpp
    composition/Minivi.cpp
    modules/HmiModules.cpp
    state/HmiTypes.cpp
    shell/HmiController.cpp
    shell/HmiRenderer.cpp
    state/HmiScreens.cpp
    modules/HomeModule.cpp
    modules/MediaPlayerModule.cpp
    modules/RadioModule.cpp
    modules/NavigationModule.cpp
    modules/PhoneModule.cpp
    modules/ClimateModule.cpp
    modules/SettingsModule.cpp
    modules/ProfileModule.cpp
    modules/VehicleModule.cpp
    modules/NotificationsModule.cpp
    ui/UiPrimitives.cpp
    shell/TerminalInput.cpp
    shell/HmiApp.cpp
)

target_link_libraries(minivi_console_hmi PRIVATE
    ${MINIVI_HMI_LIBS}
)

target_include_directories(minivi_console_hmi PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/client
    ${CMAKE_CURRENT_SOURCE_DIR}/composition
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/shell
    ${CMAKE_CURRENT_SOURCE_DIR}/state
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)
