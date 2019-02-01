cmake_minimum_required(VERSION 3.10)
project(vaso2)


set(CMAKE_CXX_STANDARD 11)

file(GLOB LIB_sntp_SRC ${ESP_OPEN_RTOS_DIR}/extras/sntp/*.c)

add_library(sntp STATIC ${LIB_sntp_SRC})

target_compile_options(sntp PUBLIC ${COMMON_FLAGS} -DLWIP_DEBUG=1 "-DSNTP_SET_SYSTEM_TIME_US(S, F)=sntp_impl_set_system_time_us(S, F)" )

target_include_directories(sntp PUBLIC
            ${ESP_OPEN_RTOS_DIR}/extras/sntp
            ${ESP_OPEN_RTOS_DIR}/extras/sntp/include
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