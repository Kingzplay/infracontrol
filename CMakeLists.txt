cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include("$ENV{PICO_SDK_PATH}/pico_sdk_init.cmake")

set(PROJECT_NAME infracontrol)

project(infracontrol C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(${CMAKE_PROJECT_NAME}
    main.c
	sh1106.c
	ui.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${CMAKE_PROJECT_NAME} pico_stdlib hardware_i2c hardware_clocks hardware_timer hardware_pwm)

# enable usb communication
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
