
add_executable(sdg_test
    main.cpp
    ${getopt_if_needed}
)

set_compile_options(sdg_test)    # function defined in parent

target_include_directories(sdg_test PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(sdg_test
    instrument
    Qt6::Core
    Qt6::Network
)
