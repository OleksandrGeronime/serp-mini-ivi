cmake_minimum_required(VERSION 3.22)
project(AutomotiveMinivi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
option(SERP_BUILD_MINIVI_CONSOLE_HMI "Build the legacy minivi console HMI frontend" OFF)

set(SERP_PRODUCT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Path to mini platform product workspace" FORCE)

find_package(Serp REQUIRED CONFIG)

add_subdirectory(build/gen/services)

set(MINIVI_HMI_LIBS
    serp
    persistencehal
    audiohal
    radiohal
    vehiclehal
    climatehal
    phonehal
    locationhal
    persistenceservice
    carnotificationmanager
    carpropertyservice
    carpowermanager
    carusermanager
    carclimatemanager
    carnavigationmanager
    carphonemanager
    caraudiomanager
    carmediamanager
    carradiomanager
)

if(SERP_BUILD_MINIVI_CONSOLE_HMI)
    add_subdirectory(src/hmi/console)
endif()
