cmake_minimum_required(VERSION 3.16)
project(repository VERSION 1.7.0 LANGUAGES CXX)
message(STATUS "project: " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Network Widgets)
#find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS WinExtras)

qt_standard_project_setup()


add_subdirectory(eyes-thanks)
#add_subdirectory(qconsole-test)
