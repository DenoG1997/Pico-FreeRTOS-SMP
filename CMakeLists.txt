cmake_minimum_required(VERSION 3.12)
# Set any variables required for importing libraries
SET(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/FreeRTOS-Kernel)

# Name project
SET(ProjectName Pico-CarX)

set(PICO_SDK_PATH "/Users/denizyildirir/pico/pico-sdk")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Import those libraries
include(pico_sdk_import.cmake)
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)
#set(PICO_DEOPTIMIZED_DEBUG on)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(OPENOCD_PATH "/Users/denizyildirir/src/openocd" CACHE STRING "Where to find openocd")
set(OPENOCD_INTERFACE "cmsis-dap" CACHE STRING "Interface to use ")


set(WARNINGS_AS_ERRORS false)
include("cmake/StandardProjectSettings.cmake")
include("cmake/mcu_command_targets.cmake")

# Define project
project(${ProjectName} C CXX ASM)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(servo)
add_subdirectory(SR04)

# if you have anything in "lib" folder then uncomment below - remember to add a CMakeLists.txt
# file to the "lib" directory
#add_subdirectory(lib)