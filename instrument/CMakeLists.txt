add_library(instrument
    ScpiConnection.cpp
    ScpiConnection.h
    SDG2000X.cpp
    SDG2000X.h
)

target_link_libraries(instrument
    Qt6::Core
    Qt6::Network
)

option(SDG_DEBUG "Enable SDG SCPI debug output" OFF)

if(SDG_DEBUG)
    target_compile_definitions(instrument PRIVATE SDG_DEBUG)
endif()

target_include_directories(instrument PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
