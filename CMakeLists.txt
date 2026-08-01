cmake_minimum_required(VERSION 3.16)

project(sdg-control)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_subdirectory(instrument)
add_subdirectory(test_scpi)
