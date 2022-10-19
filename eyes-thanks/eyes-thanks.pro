#----------------------------------------------------------------------------------#
#      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  #
#      This file is part of Eyes' Thanks.                                          #
#      GNU General Public License 3                                                #
#----------------------------------------------------------------------------------#

include("functions.pri")
#include("../qutfstring/include.pri")
message(" ")
message("$$BUILD_TIME eyes-thanks.pro")

# For NetUserGetInfo()
LIBS += -ladvapi32
LIBS += -lnetapi32


APP_NAME  = "Eyes’ Thanks"
VERSION   = 1.6.1
DEV_NAME  = Alexander Yalov
DEV_EMAIL = alexander.yalov@gmail.com
REPO_URL  = https://github.com/yalov/eyes-thanks
PATREON_URL  = https://patreon.com/yalov

win32-msvc*: TARGET    = "EyesThanks"
win32-g++*: TARGET     = "EyesThanks"

CONFIG  += DEPLOY

# OpenSSL binary provided by the Qt Maintenance Tool
# Qt/Developer and designer tools/OpenSSL Toolkit/binaries



TEMPLATE = app

QT      += core gui network widgets

lessThan(QT_MAJOR_VERSION, 6){
    QT += winextras

    CONFIG(release, debug|release) {
        win32-g++* {
            contains(QT_ARCH, i386) {
                message("$$BUILD_TIME mingw x86 build")
                DESTDIR  =   $$PWD/../../EyesThanksX86
                SSLDLLDIR = $$[QT_INSTALL_PREFIX]/../../Tools/OpenSSL/Win_x86/bin
            } else {
                message("$$BUILD_TIME mingw x86_64 build")
                DESTDIR =   $$PWD/../../EyesThanksX86_64
                SSLDLLDIR = $$[QT_INSTALL_PREFIX]/../../Tools/OpenSSL/Win_x64/bin
            }
        }
        win32-msvc* {
            contains(QT_ARCH, i386) {
                message("$$BUILD_TIME msvc x86 build")
                DESTDIR =   $$PWD/../../EyesThanksMSVC32
            } else {
                message("$$BUILD_TIME msvc x86_64 build")
                DESTDIR =   $$PWD/../../EyesThanksMSVC64
            }
        }

    }
}

greaterThan(QT_MAJOR_VERSION, 5){
    message("$$BUILD_TIME qt6")
    DESTDIR =   $$PWD/../../EyesThanksQt6
    SSLDLLDIR = $$[QT_INSTALL_PREFIX]/../../Tools/OpenSSL/Win_x64/bin
}



# subfolders in debug and release folder
OBJECTS_DIR = $$OUT_PWD/.obj
MOC_DIR     = $$OUT_PWD/.moc
RCC_DIR     = $$OUT_PWD/.qrc
UI_DIR      = $$OUT_PWD/.ui

# no debug and release subfolder in debug and release folder
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target

CONFIG += c++17

win32:RC_ICONS += icons/icon.ico

QMAKE_TARGET_DESCRIPTION = $$TARGET
QMAKE_TARGET_COPYRIGHT   = $$DEV_NAME

DEFINES +=  REPO_URL='"\\\"$$REPO_URL\\\""'
DEFINES +=  APP_VERSION='"\\\"$$VERSION\\\""'
#DEFINES +=  APP_NAME='"\\\"$$APP_NAME\\\""'
DEFINES +=  DEV_NAME='"\\\"$$DEV_NAME\\\""'
DEFINES +=  DEV_EMAIL='"\\\"$$DEV_EMAIL\\\""'
DEFINES +=  PATREON_URL='"\\\"$$PATREON_URL\\\""'

# '+' will automatically be performed as the QStringBuilder '%' everywhere.
DEFINES *= QT_USE_QSTRINGBUILDER

SOURCES  += src/dialog.cpp \
            src/main.cpp \
            src/trayicon.cpp \
            src/view.cpp \
            src/updater.cpp \
            src/timer.cpp \
            src/charactersets.cpp \
            src/viewitem.cpp \
            src/viewrandompolygon.cpp


HEADERS  += src/aboutwindow.h \
            src/dialog.h \
            src/timer.h \
            src/trayicon.h \
            src/view.h \
            src/updater.h \
            src/global.h \
            src/transliteration-iso9a.h \
            src/charactersets.h \
            src/viewitem.h \
            src/viewrandompolygon.h


TRANSLATIONS += languages/lang_en.ts languages/lang_ru.ts
RESOURCES    += resource.qrc



CONFIG(release, debug|release) {
    DEPLOY {
        message("$$BUILD_TIME deploy")
        DEFINES += QT_NO_DEBUG_OUTPUT DEPLOY

        createDir($$DESTDIR/languages/)  # need to be there, tested
        copyFilesToDir($$PWD/languages/*.qm, $$DESTDIR/languages/)

        # windeployqt only for release and DEPLOY and MinGW (dynamic)
        win32-g++* {
            message("$$BUILD_TIME windeployqt")

            windeployqtInDESTDIR(--compiler-runtime --no-svg --no-system-d3d-compiler --no-translations --no-opengl-sw)
            # windeployqtInDESTDIR(--compiler-runtime --no-svg --no-system-d3d-compiler --no-translations --no-opengl-sw --no-angle)

            removeDirRecursive($$DESTDIR/bearer)

            FILENAMES = qicns.dll qico.dll qtga.dll qtiff.dll qwbmp.dll qwebp.dll
            removeFilesInDir($$DESTDIR/imageformats/, $$FILENAMES)

            copyFilesToDir($${SSLDLLDIR}/*.dll, $$DESTDIR)
            copyFilesToDir($$PWD/../ChangeLog.md, $$DESTDIR)
            message("$$DESTDIR")
        }
    }
}

DISTFILES += \
    ../ChangeLog.md \
    ../README.md
