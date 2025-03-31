cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(moba-systemmanager VERSION 0.1.0)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-4.0) 

configure_file(config.h.in config.h)

add_executable(
    moba-systemmanager

    src/activeapps.cpp
    src/automaticcontrol.cpp
    src/clock.cpp
    src/environmentcontrol.cpp
    src/frmbase.cpp
    src/frmmain.cpp
    src/main.cpp
    src/noticelogger.cpp
    src/serverdata.cpp
    src/systemcontrol.cpp
)

install(TARGETS moba-systemmanager)

target_include_directories(moba-systemmanager PUBLIC "${PROJECT_BINARY_DIR}")

target_link_libraries(moba-systemmanager mobacommon)
target_link_libraries(moba-systemmanager ${CMAKE_SOURCE_DIR}/modules/lib-msghandling/libmoba-lib-msghandling.a)
target_link_libraries(moba-systemmanager ${GTKMM_LIBRARIES})

link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/modules/lib-msghandling/src)
