#----------------------------------------------------------------------------------#
#      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  #
#      This file is part of Eyes' Thanks.                                          #
#      GNU General Public License 3                                                #
#----------------------------------------------------------------------------------#

include("functions.pri")
include("../qutfstring/include.pri")
message("$$BUILD_TIME eyes-thanks.pro")

CONFIG  += DEPLOY

APP_NAME  = Eyes’ Thanks
VERSION   = 1.3.1
DEV_NAME  = Alexander Yalov
DEV_EMAIL = alexander.yalov@gmail.com
REPO_URL  = https://github.com/yalov/eyes-thanks
TARGET    = "Eyes\' Thanks"

CONFIG(release, debug|release) {
DESTDIR =   $$PWD/../../EyesThanks
SSLDLLDIR = $$PWD/../../../openssl-1.0.2h-bin
}

# subfolders in debug and release folder
OBJECTS_DIR = $$OUT_PWD/.obj
MOC_DIR     = $$OUT_PWD/.moc
RCC_DIR     = $$OUT_PWD/.qrc
UI_DIR      = $$OUT_PWD/.ui

TEMPLATE = app
QT      += core gui network widgets winextras

# no debug and release subfolder in debug and release folder
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target

CONFIG += c++14

win32:RC_ICONS += icons/icon.ico

QMAKE_TARGET_DESCRIPTION = $$TARGET #apostrophe don't work in the taskmanager
QMAKE_TARGET_COPYRIGHT   = $$DEV_NAME

DEFINES +=    REPO_URL='"\\\"$$REPO_URL\\\""'
DEFINES += APP_VERSION='"\\\"$$VERSION\\\""'
DEFINES +=    APP_NAME='"\\\"$$APP_NAME\\\""'
DEFINES +=    DEV_NAME='"\\\"$$DEV_NAME\\\""'
DEFINES +=   DEV_EMAIL='"\\\"$$DEV_EMAIL\\\""'

# '+' will automatically be performed as the QStringBuilder '%' everywhere.
DEFINES *= QT_USE_QSTRINGBUILDER

SOURCES += src/dialog.cpp \
           src/main.cpp \
           src/trayicon.cpp \
           src/view.cpp \
           src/updater.cpp \
           src/timer.cpp \
           src/charactersets.cpp

HEADERS  += src/aboutwindow.h \
            src/dialog.h \
            src/timer.h \
            src/trayicon.h \
            src/view.h \
            src/updater.h \
            src/global.h \
            src/transliteration-iso9a.h \
            src/charactersets.h

TRANSLATIONS += languages/lang_ru.ts languages/lang_en.ts
RESOURCES    += resource.qrc


# windeployqt only release and only DEPLOY variable
CONFIG(release, debug|release) {
    DEPLOY {
        message("DEPLOY")

        DEFINES += QT_NO_DEBUG_OUTPUT DEPLOY

        windeployqtInDESTDIR(--compiler-runtime --no-svg --no-system-d3d-compiler --no-translations --no-opengl-sw --no-angle)

        removeDirRecursive($$DESTDIR/bearer)

        FILENAMES = qicns.dll qico.dll qtga.dll qtiff.dll qwbmp.dll qwebp.dll
        removeFilesInDir($$DESTDIR/imageformats/, $$FILENAMES)

        copyFilesToDir($${SSLDLLDIR}/*.dll, $$DESTDIR)

        copyFilesToDir($$PWD/languages/*.qm, $$DESTDIR/languages/)
    }
}
