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

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4):{
    CONFIG += c++11
    QT += widgets
    win32:RC_ICONS += icons/icon.ico
}else: {
    QMAKE_CXXFLAGS += -std=c++11
}


VERSION = 0.31.99
TARGET = "EyesThanks"

QMAKE_TARGET_DESCRIPTION = Eyes\' Thanks
QMAKE_TARGET_COPYRIGHT = Alexander Yalov

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += 'APP_NAME=\'\"$$TARGET\"\''

DEFINES += 'DEVELOP_NAME=\'\"$$QMAKE_TARGET_COPYRIGHT\"\''

TEMPLATE = app

SOURCES += \
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
