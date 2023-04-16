QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

greaterThan(QT_MAJOR_VERSION,4){
        TARGET_ARCH=$${QT_ARCH}
}else{
        TARGET_ARCH=$${QMAKE_HOST.arch}
}

contains(TARGET_ARCH, x86_64){
    CONFIG(debug, debug|release){
        MOC_DIR = build/x64/moc/debug
        OBJECTS_DIR = build/x64/obj/debug
        RCC_DIR = build/x64/rcc/debug
        UI_DIR = build/x64/ui/debug
        DESTDIR = bin/x64/debug
    }else{
        MOC_DIR = build/x64/moc/release
        OBJECTS_DIR = build/x64/obj/release
        RCC_DIR = build/x64/rcc/release
        UI_DIR = build/x64/ui/release
        DESTDIR = bin/x64/release
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

SOURCES += \
    main.cpp \
    AnyPlay.cpp

HEADERS += \
    AnyPlay.h

FORMS += \
    AnyPlay.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
