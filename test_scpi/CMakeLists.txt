cmake_minimum_required(VERSION 3.16)

project(sdg_test)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(sdg_test
    main.cpp
    ScpiConnection.cpp
    SDG2000X.cpp
)

target_compile_definitions(sdg_test PRIVATE SDG_DEBUG)

target_link_libraries(sdg_test
    Qt6::Core
    Qt6::Network
)
