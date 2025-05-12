cmake_minimum_required(VERSION 3.22)

enable_language(C CXX ASM)

set(MX_Include_Dirs
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/beacon/wrappers/timer
    ${CMAKE_SOURCE_DIR}/Drivers/beacon/syscfg

)

set(STM32_Drivers_Src
    ${CMAKE_SOURCE_DIR}/Drivers/beacon/wrappers/timer/timer.cpp

)

add_library(beacon INTERFACE)
target_include_directories(beacon INTERFACE ${MX_Include_Dirs})
target_sources(beacon PUBLIC ${STM32_Drivers_Src})