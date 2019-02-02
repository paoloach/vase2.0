cmake_minimum_required(VERSION 3.10)
project(vaso2)


set(CMAKE_CXX_STANDARD 11)

file(GLOB LIB_dht_SRC ${ESP_OPEN_RTOS_DIR}/extras/dht/dht.c ${ESP_OPEN_RTOS_DIR}/extras/dht/dht.h)

add_library(dht STATIC ${LIB_dht_SRC})

target_compile_options(dht PUBLIC ${COMMON_FLAGS})

target_include_directories(dht PUBLIC
        ${ESP_OPEN_RTOS_DIR}/extras/dht
        ${ESP_OPEN_RTOS_DIR}/include
        ${ESP_OPEN_RTOS_DIR}/libc/xtensa-lx106-elf/include
        ${FREE_RTOS}/include
        ${ESP_OPEN_RTOS_DIR}/FreeRTOS/Source/include
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