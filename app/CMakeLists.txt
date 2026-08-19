add_executable(sdg-control
    main.cpp
    MainWindow.cpp
    MainWindow.h
    FrontPanelWindow.cpp
    FrontPanelWindow.h
    ChannelWidget.cpp
    ChannelWidget.h
    SettingsIO.cpp
    SettingsIO.h
    ${getopt_if_needed}
)

target_include_directories(sdg-control PRIVATE ${CMAKE_BINARY_DIR})

set_compile_options(sdg-control)    # function defined in parent

target_link_libraries(sdg-control
    instrument
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

install(TARGETS sdg-control RUNTIME DESTINATION bin)
