cmake_minimum_required(VERSION 3.16)
project(Power-Logger C ASM)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG_MODE)
endif()

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMMON_FLAGS
    -mcpu=cortex-m0plus
    -mthumb
    -mfloat-abi=soft
    -ffreestanding
    -fdata-sections
    -ffunction-sections
)

string(JOIN " " COMMON_FLAGS_STR ${COMMON_FLAGS})
set(CMAKE_C_FLAGS "${COMMON_FLAGS_STR} -O2 -Wall")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS_STR}")

set(LINKER_FLAGS
    -T${CMAKE_SOURCE_DIR}/linker/samd21e17a_flash.ld
    -nostdlib
    -nostartfiles
    -Wl,--gc-sections
)

string(JOIN " " LINKER_FLAGS_STR ${LINKER_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${LINKER_FLAGS_STR}")

include_directories(
    include
    vendor-header-files/samd21-core/samd21a/include
    vendor-header-files/arm-core/CMSIS/Core/Include
)

set(SRC
    src/main.c
)

option(USING_SAMD21E17A "Compile driver for SAMD21E17A" ON)

if(USING_SAMD21E17A)
    message(STATUS "Including SAMD21E17A sources")

    file(GLOB SAMD21E17A_STARTUP
        "${CMAKE_CURRENT_SOURCE_DIR}/src/startup/samd21e/*.c"
    )

    file(GLOB SAMD21E17A_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/src/drivers/samd21e/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/interrupts/samd21e/*.c"
    )

    list(APPEND SRC ${SAMD21E17A_SOURCES} ${SAMD21E17A_STARTUP})
endif()

add_executable(${PROJECT_NAME}.elf ${SRC})

if(USING_SAMD21E17A)
    target_compile_definitions(${PROJECT_NAME}.elf PRIVATE USING_SAMD21E17A_MCU)
endif()

add_custom_command(
    TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY}
        -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY}
        -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
)

# J-Link
add_custom_target(flash
    COMMAND JLinkExe
        -device ATSAMD21E17A
        -if SWD -speed 4000
        -AutoConnect 1
        -CommandFile ${CMAKE_SOURCE_DIR}/flash.jlink
    DEPENDS ${PROJECT_NAME}.elf
)
