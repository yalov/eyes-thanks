project(eyes-thanks)
message(STATUS "project: " ${PROJECT_NAME})

# search all source and header files
file(GLOB source_files src/*.c src/*.cpp)
file(GLOB header_files src/*.h src/*.hpp)

find_package(Qt6 COMPONENTS Widgets LinguistTools
REQUIRED)

qt_create_translation(QM_FILES
 ${CMAKE_SOURCE_DIR} languages/lang_en.ts languages/lang_ru.ts)

qt_add_executable(${PROJECT_NAME}
    ${source_files} ${header_files} ${QM_FILES}
)

#target_compile_definitions(eyes-thanks PRIVATE
#    #QT_USE_QSTRINGBUILDER
#    "APP_VERSION = \"1\""
#    "DEV_EMAIL=\"1\""
#    "DEV_NAME=\"1\""
#    "PATREON_URL=\"1\""
#    "REPO_URL=\"1\""
#)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core Qt::Gui Qt::Network Qt::Widgets advapi32 netapi32
)


# Resources:
set(res_files
    "copying.html"
    "fonts/FreeSerif Mod.otf"
    "fonts/UKIJ Diwani Yantu Mod.otf"
    "icons/actions/application-exit.png"
    "icons/actions/configure.png"
    "icons/actions/help-about.png"
    "icons/actions/media-playback-pause.png"
    "icons/actions/media-playback-start.png"
    "icons/actions/media-skip-forward.png"
    "icons/actions/save.png"
    "icons/actions/system-software-update.png"
    "icons/actions/video-display.png"
    "icons/dark/00.png"
    "icons/dark/12.png"
    "icons/dark/25.png"
    "icons/dark/37.png"
    "icons/dark/50.png"
    "icons/dark/62.png"
    "icons/dark/75.png"
    "icons/dark/87.png"
    "icons/dark/95.png"
    "icons/dark/pp.png"
    "icons/light/00.png"
    "icons/light/12.png"
    "icons/light/25.png"
    "icons/light/37.png"
    "icons/light/50.png"
    "icons/light/62.png"
    "icons/light/75.png"
    "icons/light/87.png"
    "icons/light/95.png"
    "icons/light/pp.png"
    "icons/logo.png"
    "icons/tooltip.png"
    "icons/white/00.png"
    "icons/white/12.png"
    "icons/white/25.png"
    "icons/white/37.png"
    "icons/white/50.png"
    "icons/white/62.png"
    "icons/white/75.png"
    "icons/white/87.png"
    "icons/white/95.png"
    "icons/white/pp.png"
    "res/character_sets.xml"
    "res/iso9a.txt"
)

qt_add_resources(${PROJECT_NAME} "resource"
    PREFIX "/" FILES ${res_files}
)


# qt_add_resources(SOURCES resource.qrc)

message(STATUS "CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #include(InstallRequiredSystemLibraries)
endif()

#add_definitions( -DQT_USE_STRINGBUILDER  )


set(DEPLOY off)
message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND DEPLOY)
    message(STATUS "DEPLOY: " ${DEPLOY})
    #target_compile_definitions(${PROJECT_NAME} PRIVATE
    #    DEPLOY
    #    QT_NO_DEBUG_OUTPUT
    #)
endif()

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
