cmake_minimum_required(VERSION 3.16)
project(Power-Logger C ASM)

# Toolchain settings
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler flags
set(COMMON_FLAGS "-mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -ffreestanding -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -O2 -Wall")
set(CMAKE_EXE_LINKER_FLAGS "-T${CMAKE_SOURCE_DIR}/linker/samd21e17a_flash.ld -nostartfiles -Wl,--gc-sections")

# Include directories
include_directories(include
                    samd21a/include
)

# Source files
set(SRC
    src/main.c
)

add_executable(${PROJECT_NAME}.elf ${SRC})

# Create HEX and BIN after build
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
)

# Flash target (J-Link)
add_custom_target(flash
    COMMAND JLinkExe -device ATSAMD21E17A -if SWD -speed 4000 -AutoConnect 1 -CommandFile ${CMAKE_SOURCE_DIR}/flash.jlink
    DEPENDS ${PROJECT_NAME}.elf
)
