TEMPLATE = lib
TARGET = qutfstring

CONFIG += staticlib
CONFIG += c++14
QT     -= gui

# no debug and release subfolder in debug and release folder
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target

OBJECTS_DIR = $$OUT_PWD/.obj

HEADERS += \
    QUtfChar.h \
    QUtfRegExp.h \
    QUtfString.h \
    QUtfStringList.h

SOURCES += \
    QUtfChar.cpp \
    QUtfRegExp.cpp \
    QUtfString.cpp \
    QUtfStringList.cpp
