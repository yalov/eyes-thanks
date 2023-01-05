TEMPLATE = app
TARGET = ConsoleAppTest

INCLUDEPATH += $$PWD/../eyes-thanks/src

# For NetUserGetInfo()
LIBS += -ladvapi32
LIBS += -lnetapi32

QT -= gui
QT += widgets

CONFIG += c++17
CONFIG += console
CONFIG -= app_bundle

CONFIG += municode

SOURCES += \
    main.cpp \
    main2.cpp \
    main3.cpp

# subfolders in debug and release folder
OBJECTS_DIR = $$OUT_PWD/.obj
MOC_DIR     = $$OUT_PWD/.moc
RCC_DIR     = $$OUT_PWD/.qrc
UI_DIR      = $$OUT_PWD/.ui

# no debug and release subfolder in debug and release folder
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target
