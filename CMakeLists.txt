cmake_minimum_required(VERSION 3.16)
project(repository LANGUAGES CXX)
message(STATUS "project: " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
message(STATUS "QT_VERSION_MAJOR: " ${QT_VERSION_MAJOR})

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Network Widgets)

if (Qt6_FOUND)
    qt_standard_project_setup()
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
endif()



add_subdirectory(eyes-thanks)
#add_subdirectory(qconsole-test)
