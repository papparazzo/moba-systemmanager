cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(moba-systemmanager VERSION 0.1.0)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-4.0) 

configure_file(config.h.in config.h)

add_executable(
    moba-systemmanger

    src/ctrlambience.cpp     
    src/ctrlambientlight.cpp  
    src/ctrlblocklock.cpp     
    src/ctrlbool.cpp          
    src/ctrlcolortheme.cpp    
    src/ctrlconnectivity.cpp  
    src/ctrlcontact.cpp       
    src/ctrlenvironment.cpp     
    src/ctrlglobaltimer.cpp     
    src/ctrlsetbrakevector.cpp  
    src/ctrlstring.cpp
    src/ctrltracklayout.cpp
    src/frmmain.cpp
    src/main.cpp
    src/msgsender.cpp
)

install(TARGETS moba-systemmanager)

target_include_directories(moba-tester PUBLIC "${PROJECT_BINARY_DIR}")

target_link_libraries(moba-tester mobacommon)
target_link_libraries(moba-tester ${CMAKE_SOURCE_DIR}/modules/lib-msghandling/libmoba-lib-msghandling.a)
target_link_libraries(moba-tester ${GTKMM_LIBRARIES})

link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/modules/lib-msghandling/src)
