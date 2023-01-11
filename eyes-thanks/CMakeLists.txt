project(eyes-thanks)
message(STATUS "project: " ${PROJECT_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(APP_NAME "Eyes’ Thanks")
set(VERSION "1.5.4")
set(DEV_NAME "Alexander Yalov")
set(DEV_EMAIL "alexander.yalov@gmail.com")
set(REPO_URL "https://github.com/yalov/eyes-thanks")
set(PATREON_URL "https://patreon.com/yalov")


# search all source and header files
file(GLOB source_files src/*.c src/*.cpp)
file(GLOB header_files src/*.h src/*.hpp)
file(GLOB ts_files languages/*.ts)

qt_add_resources(resources resource.qrc)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS LinguistTools REQUIRED)
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${ts_files})


# icon of the exe
enable_language("RC")
set (ICON_RES ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon.o)

if (Qt6_FOUND)
    qt_add_executable(${PROJECT_NAME}
        ${source_files} ${header_files} ${QM_FILES} ${resources} ${ICON_RES}
    )
else()
    add_executable(${PROJECT_NAME} WIN32
        ${source_files} ${header_files} ${QM_FILES} ${resources} ${ICON_RES}
    )
endif()


target_compile_definitions(${PROJECT_NAME} PUBLIC APP_VERSION="\"\"${VERSION}\"\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC DEV_NAME="\"\"${DEV_NAME}\"\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC DEV_EMAIL="\"\"${DEV_EMAIL}\"\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC REPO_URL="\"\"${REPO_URL}\"\"")
target_compile_definitions(${PROJECT_NAME} PUBLIC PATREON_URL="\"\"${PATREON_URL}\"\"")


target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core Qt::Gui Qt::Network Qt::Widgets advapi32 netapi32
)

#set (CMAKE_CXX_FLAGS "-O2")

message(STATUS "CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})

#add_definitions( -DQT_USE_STRINGBUILDER  )


#set(DEPLOY off)

message(STATUS "CMAKE_INSTALL_BINDIR: " ${CMAKE_INSTALL_BINDIR})
message(STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
message(STATUS "CMAKE_HOME_DIRECTORY: " ${CMAKE_HOME_DIRECTORY})


set (DEPLOY_DIR ${CMAKE_SOURCE_DIR}/../EyesThanksCmakeQt5)
set (CMAKE_INSTALL_BINDIR ${DEPLOY_DIR})

install(DIRECTORY DESTINATION ${DEPLOY_DIR})

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${DEPLOY_DIR}
)


#qt_generate_deploy_app_script(
#    TARGET ${PROJECT_NAME}
#    FILENAME_VARIABLE deploy_script
#    NO_UNSUPPORTED_PLATFORM_ERROR
#)
#message(STATUS "deploy_script: " ${deploy_script})
#install(SCRIPT ${deploy_script})


#set(deploy_script "${CMAKE_CURRENT_BINARY_DIR}/deploy_${PROJECT_NAME}.cmake")
#message(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
#message(STATUS "QT_DEPLOY_PREFIX: " "${QT_DEPLOY_PREFIX}")


#file(GENERATE OUTPUT ${deploy_script} CONTENT "
#include(\"${QT_DEPLOY_SUPPORT}\")
#
#qt_deploy_runtime_dependencies(
#    EXECUTABLE \"\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>\"
#
#)")
#install(SCRIPT ${deploy_script})



if (false)
    get_target_property(QMAKE_EXECUTABLE Qt::qmake IMPORTED_LOCATION)
    get_filename_component(WINDEPLOYQT_EXECUTABLE ${QMAKE_EXECUTABLE} PATH)
    set(WINDEPLOYQT_EXECUTABLE "${WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")
    message(STATUS "WINDEPLOYQT_EXECUTABLE: " "${WINDEPLOYQT_EXECUTABLE}")

    set(DEPLOY_EXE ${DEPLOY_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>)
    message(STATUS "DEPLOY_EXE: " "${DEPLOY_EXE}")

   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
       COMMAND ${WINDEPLOYQT_EXECUTABLE} ${DEPLOY_EXE})
endif()
