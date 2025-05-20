cmake_minimum_required(VERSION 3.22)

enable_language(C CXX ASM)

set(MX_Include_Dirs
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/beacon/syscfg

    ${CMAKE_SOURCE_DIR}/Drivers/beacon/wrappers/i2c_master

    ${CMAKE_SOURCE_DIR}/Drivers/beacon/hardware_objects/debug_led
    ${CMAKE_SOURCE_DIR}/Drivers/beacon/hardware_objects/ltr_303als
    ${CMAKE_SOURCE_DIR}/Drivers/beacon/hardware_objects/neopixel_4310

)

set(STM32_Drivers_Src
    ${CMAKE_SOURCE_DIR}/Core/Src/cpp_main.cpp

    ${CMAKE_SOURCE_DIR}/Drivers/beacon/wrappers/i2c_master/i2c_master.cpp
    
    ${CMAKE_SOURCE_DIR}/Drivers/beacon/hardware_objects/debug_led/debug_led.cpp
    ${CMAKE_SOURCE_DIR}/Drivers/beacon/hardware_objects/ltr_303als/ltr_303als.cpp
    ${CMAKE_SOURCE_DIR}/Drivers/beacon/hardware_objects/neopixel_4310/neopixel_4310.cpp

)

add_library(beacon INTERFACE)
target_include_directories(beacon INTERFACE ${MX_Include_Dirs})
target_sources(beacon PUBLIC ${STM32_Drivers_Src})