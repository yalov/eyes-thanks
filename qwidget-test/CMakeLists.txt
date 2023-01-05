cmake_minimum_required(VERSION 3.16)
project(qwidget-test VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(qwidget-test WIN32 MACOSX_BUNDLE
    main.cpp
    widget.cpp widget.h
)
target_compile_definitions(qwidget-test PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(qwidget-test PRIVATE
    Qt::Core
    Qt::Gui
    advapi32
    netapi32
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(qwidget-test PRIVATE
        Qt::Widgets
    )
endif()

install(TARGETS qwidget-test
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET qwidget-test
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
