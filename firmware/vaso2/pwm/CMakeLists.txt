cmake_minimum_required(VERSION 3.10)
project(vaso2)


set(CMAKE_CXX_STANDARD 11)

file(GLOB LIB_pwm_SRC ${ESP_OPEN_RTOS_DIR}/extras/pwm/*.c)

add_library(pwm STATIC ${LIB_pwm_SRC})

target_compile_options(pwm PUBLIC ${COMMON_FLAGS} )

target_include_directories(pwm PUBLIC
            ${ESP_OPEN_RTOS_DIR}/extras/pwm
            ${ESP_OPEN_RTOS_DIR}/extras/pwm/include
            ${ESP_OPEN_RTOS_DIR}/include
            ${ESP_OPEN_RTOS_DIR}/libc/xtensa-lx106-elf/include
            ${FREE_RTOS}/include
            ${FREE_RTOS}/portable/esp8266
            ${LWIP_DIR}/include
            ${ESP_OPEN_RTOS_DIR}/lwip/include
            ${LWIP_DIR}/include/compat/posix
            ${LWIP_DIR}/include/ipv4
            ${LWIP_DIR}/include/ipv4/lwip
            ${LWIP_DIR}/include/lwip
            ${ESP_OPEN_RTOS_DIR}/core/include
            ${ESP_OPEN_RTOS_DIR}/open_esplibs/include
            )