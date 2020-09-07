cmake_minimum_required(VERSION 2.8)
project(jcu-daemon CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/installer.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/src/daemon-impl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/daemon-impl.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/jcu-daemon/daemon.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/jcu-daemon/installer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform-daemon.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform-installer.h
        )

if (WIN32)
    set(SRC_PLATFORM_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/service-base.h
            ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/service-base.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/windows-daemon.h
            ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/windows-daemon.cc
            ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/windows-installer.h
            ${CMAKE_CURRENT_SOURCE_DIR}/src/windows/windows-installer.cc
            )
else()
    set(SRC_PLATFORM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/linux)
    set(SRC_PLATFORM_FILES
            ${SRC_PLATFORM_DIR}/linux_daemon.h
            ${SRC_PLATFORM_DIR}/linux_daemon.cc
            )
endif ()

add_library(${PROJECT_NAME} ${SRC_FILES} ${SRC_PLATFORM_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

target_link_libraries(${PROJECT_NAME} jcu-file)
