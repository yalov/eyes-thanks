#----------------------------------------------------------------------------------#
#      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  #
#      This file is part of Eyes' Thanks.                                          #
#      GNU General Public License 3                                                #
#----------------------------------------------------------------------------------#

include("functions.pri")
StartProjectMESSAGE()

#CONFIG  += DEPLOY
APP_NAME  = Eyes’ Thanks
VERSION   = 1.2.2
DEV_NAME  = Alexander Yalov
DEV_EMAIL = alexander.yalov@gmail.com
REPO_URL  = https://github.com/yalov/eyes-thanks
TARGET    = "Eyes\' Thanks"

CONFIG(release, debug|release) {
DESTDIR = $$PWD/../EyesThanks
}

# subfolders in debug and release folder
OBJECTS_DIR = $$OUT_PWD/.obj
MOC_DIR     = $$OUT_PWD/.moc
RCC_DIR     = $$OUT_PWD/.qrc
UI_DIR      = $$OUT_PWD/.ui

# no debug and release subfolder in debug and release folder
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target

CONFIG += c++11

QT     += core gui network widgets winextras

win32:RC_ICONS += icons/icon.ico

QMAKE_TARGET_DESCRIPTION = $$TARGET
QMAKE_TARGET_COPYRIGHT   = $$DEV_NAME

DEFINES +=    REPO_URL='"\\\"$$REPO_URL\\\""'
DEFINES += APP_VERSION='"\\\"$$VERSION\\\""'
DEFINES +=    APP_NAME='"\\\"$$APP_NAME\\\""'
DEFINES +=    DEV_NAME='"\\\"$$DEV_NAME\\\""'
DEFINES +=   DEV_EMAIL='"\\\"$$DEV_EMAIL\\\""'

# '+' will automatically be performed as the QStringBuilder '%' everywhere.
DEFINES *= QT_USE_QSTRINGBUILDER

TEMPLATE = app

SOURCES += \
    src/dialog.cpp \
    src/main.cpp \
    src/trayicon.cpp \
    src/view.cpp \
    src/updater.cpp \
    src/timer.cpp

HEADERS  += src/aboutwindow.h \
    src/dialog.h \
    src/timer.h \
    src/trayicon.h \
    src/view.h \
    src/updater.h \
    src/global.h \
    src/transliteration-iso9a.h

TRANSLATIONS += languages/lang_ru.ts languages/lang_en.ts
RESOURCES    += resource.qrc

# windeployqt only release and only DEPLOY variable
CONFIG(release, debug|release) {
    DEPLOY {
        message( "qt_no_debug_output" )
        DEFINES += QT_NO_DEBUG_OUTPUT DEPLOY
        message( "start windeployqt" )

        windeployqtInDESTDIR(--compiler-runtime --no-svg --no-system-d3d-compiler --no-translations --no-opengl-sw --no-angle)

        removeDirRecursive($$DESTDIR\bearer)

        FILES_TO_DEL = $$DESTDIR/imageformats/qicns.dll \
                       $$DESTDIR/imageformats/qico.dll \
                       $$DESTDIR/imageformats/qtga.dll \
                       $$DESTDIR/imageformats/qtiff.dll \
                       $$DESTDIR/imageformats/qwbmp.dll \
                       $$DESTDIR/imageformats/qwebp.dll
        removeFiles($$FILES_TO_DEL)

        copyFilesToDESTDIR($$PWD/../../openssl-1.0.2h-bin/*.dll)

        # create language folder and copy *.qm
        RETURN = $$escape_expand(\n\t)
        LANGFILES = $$shell_path($$PWD/languages/*.qm)
        LANGDIR = $$shell_path($$DESTDIR/languages/)
        QMAKE_POST_LINK += $$RETURN $$sprintf($$QMAKE_MKDIR_CMD, $$LANGDIR)
        QMAKE_POST_LINK += $$RETURN $$QMAKE_COPY $$quote($$LANGFILES) $$quote($$LANGDIR)

    }
}
