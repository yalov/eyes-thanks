add_executable(ConsoleAppTest
    main.cpp
    main2.cpp
    main3.cpp
)
target_include_directories(ConsoleAppTest PRIVATE
    ../eyes-thanks/src
)

target_link_libraries(ConsoleAppTest PRIVATE
    Qt::Core
    Qt::Widgets
    advapi32
    netapi32
)

install(TARGETS ConsoleAppTest
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ConsoleAppTest
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
