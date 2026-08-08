
option(SDG_DEBUG "Enable SDG SCPI debug output" OFF)

add_library(instrument
    ScpiConnection.cpp
    ScpiConnection.h
    SDG2000X.cpp
    SDG2000X.h
    Instrument.h
)

if ( SDG_DEBUG )
    target_compile_definitions(instrument PRIVATE SDG_DEBUG)
    target_compile_options(instrument PRIVATE
        -Wextra -Wmisleading-indentation -Wduplicated-cond -Wlogical-op
        -Wnull-dereference -Wshadow -Wunused -Wsizeof-array-argument
        -Wduplicated-branches -Wparentheses)
endif ()

target_include_directories(instrument PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(instrument PUBLIC ${PROJECT_SOURCE_DIR}/common)

target_link_libraries(instrument
    Qt6::Core
    Qt6::Network
)

target_include_directories(instrument PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
