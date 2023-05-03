QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE += $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

#win32{
#
#}
#win64{
#    include(breakpad/breakpad.pri)
#}

INCLUDEPATH += $$PWD/3rdpart
INCLUDEPATH += $$PWD/Config
INCLUDEPATH += $$PWD/Decode
INCLUDEPATH += $$PWD/Global
INCLUDEPATH += $$PWD/Log
INCLUDEPATH += $$PWD/Timer



greaterThan(QT_MAJOR_VERSION,4){
        TARGET_ARCH=$${QT_ARCH}
}else{
        TARGET_ARCH=$${QMAKE_HOST.arch}
}

win32 {
    #include(3rdpart/breakpad/breakpad.pri)

    contains(QMAKE_HOST.arch, x86_64) {
        LIBS += -L $$PWD/ffmpeg/win64/lib -lavcodec -lavformat -lavutil -lswscale -lswresample
        INCLUDEPATH += $$PWD/ffmpeg/win64/include

        INCLUDEPATH +=  $$PWD/3rdpart/SDL2/win64/include
        LIBS += -L$$PWD/3rdpart/SDL2/win64/lib -lSDL2

        CONFIG(debug, debug|release){
            MOC_DIR = build/win64/moc/debug
            OBJECTS_DIR = build/win64/obj/debug
            RCC_DIR = build/win64/rcc/debug
            UI_DIR = build/win64/ui/debug
            DESTDIR = bin/win64/debug
        }else{
            MOC_DIR = build/win64/moc/release
            OBJECTS_DIR = build/win64/obj/release
            RCC_DIR = build/win64/rcc/release
            UI_DIR = build/win64/ui/release
            DESTDIR = bin/win64/release
        }
    }else{
        CONFIG(debug, debug|release){
            MOC_DIR = build/win32/moc/debug
            OBJECTS_DIR = build/win32/obj/debug
            RCC_DIR = build/win32/rcc/debug
            UI_DIR = build/win32/ui/debug
            DESTDIR = bin/win32/debug
        }else{
            MOC_DIR = build/win32/moc/release
            OBJECTS_DIR = build/win32/obj/release
            RCC_DIR = build/win32/rcc/release
            UI_DIR = build/win32/ui/release
            DESTDIR = bin/win32/release
        }
    }
}

linux-* {
    contains(QMAKE_HOST.arch, x86_64) {
    # Linux 64-bit specific settings
    #LIBS += -L/path/to/linux64/libraries
    #INCLUDEPATH += /path/to/linux64/includes

        CONFIG(debug, debug|release){
            MOC_DIR = build/linux64/moc/debug
            OBJECTS_DIR = build/linux64/obj/debug
            RCC_DIR = build/linux64/rcc/debug
            UI_DIR = build/linux64/ui/debug
            DESTDIR = bin/linux64/debug
        }else{
            MOC_DIR = build/linux64/moc/release
            OBJECTS_DIR = build/linux64/obj/release
            RCC_DIR = build/linux64/rcc/release
            UI_DIR = build/linux64/ui/release
            DESTDIR = bin/linux64/release
        }
    } else {
        # Linux 32-bit specific settings
        #LIBS += -L/path/to/linux32/libraries
        #INCLUDEPATH += /path/to/linux32/includes

        CONFIG(debug, debug|release){
            MOC_DIR = build/linux32/moc/debug
            OBJECTS_DIR = build/linux32/obj/debug
            RCC_DIR = build/linux32/rcc/debug
            UI_DIR = build/linux32/ui/debug
            DESTDIR = bin/linux32/debug
        }else{
            MOC_DIR = build/linux32/moc/release
            OBJECTS_DIR = build/linux32/obj/release
            RCC_DIR = build/linux32/rcc/release
            UI_DIR = build/linux32/ui/release
            DESTDIR = bin/linux32/release
        }
    }
}

macx {
#    # macOS-specific settings
#    LIBS += -L/path/to/macOS/libraries
#    INCLUDEPATH += /path/to/macOS/includes

    CONFIG(debug, debug|release){
        MOC_DIR = build/linux32/moc/debug
        OBJECTS_DIR = build/linux32/obj/debug
        RCC_DIR = build/linux32/rcc/debug
        UI_DIR = build/linux32/ui/debug
        DESTDIR = bin/linux32/debug
    }else{
        MOC_DIR = build/linux32/moc/release
        OBJECTS_DIR = build/linux32/obj/release
        RCC_DIR = build/linux32/rcc/release
        UI_DIR = build/linux32/ui/release
        DESTDIR = bin/linux32/release
    }
}

SOURCES += \
    Decode/AudioDecode.cpp \
    Decode/Decode.cpp \
    Decode/VideoDecode.cpp \
    Timer/UpateTimer.cpp \
    VideoPlayWidget.cpp \
    Global/GlobalVar.cpp \
    main.cpp \
    AnyPlay.cpp

HEADERS += \
    AnyPlay.h \
    Decode/AudioDecode.h \
    Decode/Decode.h \
    Decode/VideoDecode.h \
    Log/Logger.hpp \
    Timer/UpateTimer.h \
    VideoPlayWidget.h \
    Config/INIReader.hpp \
    ffmpegIn.h \
    Global/GlobalVar.h

FORMS += \
    AnyPlay.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
