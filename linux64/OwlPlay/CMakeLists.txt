cmake_minimum_required(VERSION 3.21)

project(OwlPlay VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Core Gui OpenGL Multimedia REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Core Gui OpenGL Multimedia REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        OwlAudioPlay.h  OwlAudioPlay.cpp
        OwlAudioThread.h  OwlAudioThread.cpp
        OwlDecode.h  OwlDecode.cpp
        OwlDecodeThread.h  OwlDecodeThread.cpp
        OwlDemux.h  OwlDemux.cpp
        OwlDemuxThread.h  OwlDemuxThread.cpp
        OwlVideoThread.h  OwlVideoThread.cpp
        OwlPlay.h  OwlPlay.cpp
        OwlResample.h  OwlResample.cpp
        OwlSlider.h  OwlSlider.cpp
        OwlVideoWidget.h  OwlVideoWidget.cpp
        IVideoCall.h
        OwlPlay.ui
        OwlPlay.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OwlPlay
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET untitled APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(OwlPlay SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(OwlPlay
            ${PROJECT_SOURCES}
        )
    endif()
endif()

set(FFMPEG_ROOT "/usr")

target_link_libraries(OwlPlay PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::Multimedia
    "${FFMPEG_ROOT}/lib/libavutil.so"
    "${FFMPEG_ROOT}/lib/libpostproc.so"
    "${FFMPEG_ROOT}/lib/libswresample.so"
    "${FFMPEG_ROOT}/lib/libswscale.so"
    "${FFMPEG_ROOT}/lib/libavfilter.so"
    "${FFMPEG_ROOT}/lib/libavdevice.so"
    "${FFMPEG_ROOT}/lib/libavformat.so"
    "${FFMPEG_ROOT}/lib/libavcodec.so"
    "${FFMPEG_ROOT}/lib/libasound.so")

set_target_properties(OwlPlay PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OwlPlay)
endif()
