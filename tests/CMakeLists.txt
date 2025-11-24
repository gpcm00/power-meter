file(GLOB STARTUP_SRCS
    ${CMAKE_SOURCE_DIR}/src/startup/samd21e/*.c
)

add_executable(clock_driver EXCLUDE_FROM_ALL
    ${CMAKE_SOURCE_DIR}/tests/clock_driver/main.c
    ${CMAKE_SOURCE_DIR}/src/drivers/samd21e/gpio.c
    ${STARTUP_SRCS}
)

set_target_properties(clock_driver PROPERTIES
    RUNTIME_OUTPUT_NAME "clock_driver.elf"
)
