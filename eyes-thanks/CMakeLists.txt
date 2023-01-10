project(eyes-thanks)
message(STATUS "project: " ${PROJECT_NAME})

# search all source and header files
file(GLOB source_files src/*.c src/*.cpp)
file(GLOB header_files src/*.h src/*.hpp)

qt_add_resources(resources resource.qrc)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools
REQUIRED)

qt_create_translation(QM_FILES
 ${CMAKE_SOURCE_DIR} languages/lang_en.ts languages/lang_ru.ts)

if (Qt6_FOUND)
qt_add_executable(${PROJECT_NAME}
    ${source_files} ${header_files} ${QM_FILES} ${resources}
)
endif()

if (NOT Qt6_FOUND)
add_executable(${PROJECT_NAME}
    ${source_files} ${header_files} ${QM_FILES} ${resources}
)
endif()


set(APP_NAME "Eyes’ Thanks")
set(VERSION "1.5.4")
set(DEV_NAME "Alexander Yalov")
set(DEV_EMAIL "alexander.yalov@gmail.com")
set(REPO_URL "https://github.com/yalov/eyes-thanks")
set(PATREON_URL "https://patreon.com/yalov")


target_compile_definitions(eyes-thanks PUBLIC APP_VERSION="\"\"${VERSION}\"\"")
target_compile_definitions(eyes-thanks PUBLIC DEV_NAME="\"\"${DEV_NAME}\"\"")
target_compile_definitions(eyes-thanks PUBLIC DEV_EMAIL="\"\"${DEV_EMAIL}\"\"")
target_compile_definitions(eyes-thanks PUBLIC REPO_URL="\"\"${REPO_URL}\"\"")
target_compile_definitions(eyes-thanks PUBLIC PATREON_URL="\"\"${PATREON_URL}\"\"")


target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core Qt::Gui Qt::Network Qt::Widgets advapi32 netapi32
)



message(STATUS "CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #include(InstallRequiredSystemLibraries)
endif()

#add_definitions( -DQT_USE_STRINGBUILDER  )


set(DEPLOY off)

#install(TARGETS ${PROJECT_NAME}
#    BUNDLE DESTINATION .
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#)


#install(TARGETS ${PROJECT_NAME}
#        CONFIGURATIONS Release
#        RUNTIME DESTINATION Release/bin)
