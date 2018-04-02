TEMPLATE = app
TARGET = ConsoleAppTest

INCLUDEPATH += $$PWD/../eyes-thanks/src

QT -= gui
QT += widgets

CONFIG += c++14
CONFIG += console
CONFIG -= app_bundle

SOURCES += main.cpp

# subfolders in debug and release folder
OBJECTS_DIR = $$OUT_PWD/.obj
MOC_DIR     = $$OUT_PWD/.moc
RCC_DIR     = $$OUT_PWD/.qrc
UI_DIR      = $$OUT_PWD/.ui

# no debug and release subfolder in debug and release folder
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target
