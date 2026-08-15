add_executable(sdg_test
    main.cpp
)

target_include_directories(sdg_test PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(sdg_test
    instrument
    Qt6::Core
    Qt6::Network
)
