#-------------------------------------------------
#
# Project created by QtCreator 2015-08-07T20:57:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4):{
    CONFIG += c++11
    QT += widgets
    win32:RC_ICONS += icons/icon.ico
}else: {
    QMAKE_CXXFLAGS += -std=c++11
}


VERSION = 0.30.0
TARGET = "EyesThanks"

QMAKE_TARGET_DESCRIPTION = Eyes\' Thanks
QMAKE_TARGET_COPYRIGHT = Alexander Yalov

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += 'APP_NAME=\'\"$$TARGET\"\''

DEFINES += 'DEVELOP_NAME=\'\"$$QMAKE_TARGET_COPYRIGHT\"\''

TEMPLATE = app

SOURCES += src/aboutwindow.cpp \
    src/dialog.cpp \
    src/main.cpp \
    src/trayicon.cpp \
    src/view.cpp

HEADERS  += src/aboutwindow.h \
    src/dialog.h \
    src/graphicstextitemfixbound.h \
    src/timer.h \
    src/trayicon.h \
    src/view.h

TRANSLATIONS += lang_ru.ts

RESOURCES += \
    resource.qrc
