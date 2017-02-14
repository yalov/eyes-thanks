#----------------------------------------------------------------------------------#
#      Copyright 2015 Alexander Yalov <alexander.yalov@gmail.com>                  #
#                                                                                  #
#      This file is part of Eyes' Thanks.                                          #
#                                                                                  #
#      Eyes' Thanks is free software: you can redistribute it and/or modify        #
#      it under the terms of the GNU General Public License either                 #
#      version 3 of the License, or (at your option) any later version.            #
#                                                                                  #
#      Eyes' Thanks is distributed in the hope that it will be useful,             #
#      but WITHOUT ANY WARRANTY; without even the implied warranty of              #
#      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               #
#      GNU General Public License for more details.                                #
#                                                                                  #
#      You should have received a copy of the GNU General Public License           #
#      along with Eyes' Thanks.  If not, see <http://www.gnu.org/licenses/>.       #
#----------------------------------------------------------------------------------#

VERSION = 1.0.0
TARGET  = "Eyes\' Thanks"

# no debug and release subfolder in debug and release folder
CONFIG-=debug_and_release

CONFIG(release, debug|release) {
DESTDIR = ../EyesThanks
}

OBJECTS_DIR = $$OUT_PWD/.obj
MOC_DIR     = $$OUT_PWD/.moc
RCC_DIR     = $$OUT_PWD/.qrc
UI_DIR      = $$OUT_PWD/.ui



QT     += core gui network widgets winextras

CONFIG += c++11

win32:RC_ICONS += icons/icon.ico

QMAKE_TARGET_DESCRIPTION = Eyes\' Thanks
QMAKE_TARGET_COPYRIGHT   = Alexander Yalov

DEFINES += REPOSITORY_PATH='"\\\"https://github.com/yalov/eyes-thanks\\\""'

DEFINES += APP_VERSION='"\\\"$$VERSION\\\""'
#DEFINES += APP_NAME='"\\\"$$TARGET\\\""'

DEFINES += DEVELOP_NAME='"\\\"$$QMAKE_TARGET_COPYRIGHT\\\""'
DEFINES += DEVELOP_EMAIL='"\\\"alexander.yalov@gmail.com\\\""'

TEMPLATE = app

SOURCES += \
    src/dialog.cpp \
    src/main.cpp \
    src/trayicon.cpp \
    src/view.cpp \
    src/updater.cpp

HEADERS  += src/aboutwindow.h \
    src/dialog.h \
    src/graphicstextitemfixbound.h \
    src/timer.h \
    src/trayicon.h \
    src/view.h \
    src/updater.h

TRANSLATIONS += languages/lang_ru.ts
RESOURCES += resource.qrc



# windeployqt only release with removing other
CONFIG(release, debug|release) {

    defineTest(WinDeployQt) {

        # new line
        RETURN = $$escape_expand(\n\t)

        # windeployqt
        QMAKE_POST_LINK += $$RETURN windeployqt --compiler-runtime \
            --no-svg --no-system-d3d-compiler --no-translations --no-opengl-sw --no-angle $$quote($$shell_path($$DESTDIR))

        # remove bearer\ folder with files
        DIR_TO_DEL = $$shell_path($$DESTDIR\bearer)
        QMAKE_POST_LINK += $$RETURN $$QMAKE_DEL_TREE $$quote($$DIR_TO_DEL)

        # remove some file in imageformats\ folder
        DIROFFILES_TO_DEL = $$shell_path($$DESTDIR/imageformats/)
        FILES_TO_DEL = qicns.dll qico.dll qtga.dll qtiff.dll qwbmp.dll qwebp.dll

        for(FILE,FILES_TO_DEL){
            QMAKE_POST_LINK += $$RETURN $$QMAKE_DEL_FILE $$quote($$DIROFFILES_TO_DEL$$FILE)
        }

        # copy OpenSSL dlls
        OPENSSLFILES = $$shell_path($$PWD/../../bin_openssl_1.0.2h/*.dll)
        DDIR = $$shell_path($$DESTDIR)
        QMAKE_POST_LINK += $$RETURN $$QMAKE_COPY $$quote($$OPENSSLFILES) $$quote($$DDIR)


        # create language folder and copy *.qm
        LANGFILES = $$shell_path($$PWD/languages/*.qm)
        LANGDIR = $$shell_path($$DESTDIR/languages/)
        QMAKE_POST_LINK += $$RETURN $$sprintf($$QMAKE_MKDIR_CMD, $$LANGDIR)
        QMAKE_POST_LINK += $$RETURN $$QMAKE_COPY $$quote($$LANGFILES) $$quote($$LANGDIR)

        export(QMAKE_POST_LINK)

    }
    # uncomment for windeployqt:
     WinDeployQt()
}
