add_executable(sdg_control
    main.cpp
    MainWindow.cpp
    MainWindow.h
)

target_link_libraries(sdg_control
    instrument
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)
