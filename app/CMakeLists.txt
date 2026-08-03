add_executable(sdg-control
    main.cpp
    MainWindow.cpp
    MainWindow.h
    ChannelWidget.cpp
    ChannelWidget.h
)

target_include_directories(sdg-control PRIVATE ${CMAKE_BINARY_DIR})

if ( SDG_DEBUG )
    target_compile_definitions(sdg-control PRIVATE SDG_DEBUG)
endif ()

target_link_libraries(sdg-control
    instrument
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)
