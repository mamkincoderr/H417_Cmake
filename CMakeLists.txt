# =============================================================================
# H417_Cmake — шаблон для CH32H417 (два ядра: V3F ведущее, V5F ведомое).
# Автор: Дмитрий (GitHub: mamkincoderr, https://github.com/mamkincoderr)
# Telegram: https://t.me/oDeXteRo
#
# CMake + Ninja + RISC-V GCC 15 из MounRiver Studio 2. SDK — openwch/ch32h417 EVT V1.6 без правок.
# Из одного дерева собираются два образа и их склейка для прошивки одним файлом:
#   H417_V3F.elf  флеш 0x00000, 64 КБ   (Ld/Link_v3f.ld)
#   H417_V5F.elf  флеш 0x10000, 128 КБ  (Ld/Link_v5f.ld)
#   H417_Merge.hex / .bin  — оба образа, пустоты 0xFF
# =============================================================================
cmake_minimum_required(VERSION 3.20)

project(H417_Cmake C ASM)

# Флаги ядра как в EVT WCH (MRS2: B-расширение + xw, FPU в примерах выключен).
set(CPU_FLAGS
    -march=rv32imac_zba_zbb_zbc_zbs_xw
    -mabi=ilp32
    -msmall-data-limit=8
    -msave-restore
    -fmessage-length=0
    -fsigned-char
    -ffunction-sections
    -fdata-sections
    -fno-common
    -g
)

set(SDK_OPT  -Os)   # библиотека WCH
set(USER_OPT -O0)   # свой код: удобный пошаговый отладчик

file(GLOB PERIPH_SRCS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/Peripheral/src/*.c)
file(GLOB COMMON_SRCS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/Common/*.c)

# Один исполняемый файл на ядро. Библиотека собирается отдельно под каждое ядро:
# в ней есть ветки #ifdef Core_V3F / Core_V5F.
function(h417_core CORE)
    string(TOLOWER ${CORE} lc)
    set(T H417_${CORE})
    set(LD ${CMAKE_SOURCE_DIR}/Ld/Link_${lc}.ld)
    file(GLOB USER_SRCS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/User/${CORE}/*.c)

    add_library(${T}_sdk OBJECT
        ${CMAKE_SOURCE_DIR}/Startup/startup_ch32h417_${lc}.S
        ${CMAKE_SOURCE_DIR}/Core/core_riscv.c
        ${CMAKE_SOURCE_DIR}/Debug/debug.c
        ${PERIPH_SRCS})
    add_library(${T}_user OBJECT ${USER_SRCS} ${COMMON_SRCS})

    foreach(L ${T}_sdk ${T}_user)
        # printf обоих ядер — в USART1 (PA9): UART8 ядра V5F по умолчанию на PB4, а у QEU-корпуса PB4 нет.
        target_compile_definitions(${L} PRIVATE Core_${CORE} DEBUG=DEBUG_UART1)
        target_include_directories(${L} PRIVATE
            ${CMAKE_SOURCE_DIR}/User/${CORE}
            ${CMAKE_SOURCE_DIR}/Common
            ${CMAKE_SOURCE_DIR}/Core
            ${CMAKE_SOURCE_DIR}/Debug
            ${CMAKE_SOURCE_DIR}/Peripheral/inc)
        target_compile_options(${L} PRIVATE ${CPU_FLAGS} $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp>)
    endforeach()
    target_compile_options(${T}_sdk  PRIVATE $<$<COMPILE_LANGUAGE:C>:${SDK_OPT}>)
    target_compile_options(${T}_user PRIVATE ${USER_OPT} -Wall)
    # Заголовки SDK для своего кода — системные: -Wall не ругается на чужой код (ch32h417_eth.h).
    target_include_directories(${T}_user SYSTEM BEFORE PRIVATE
        ${CMAKE_SOURCE_DIR}/Core
        ${CMAKE_SOURCE_DIR}/Debug
        ${CMAKE_SOURCE_DIR}/Peripheral/inc)

    add_executable(${T} $<TARGET_OBJECTS:${T}_sdk> $<TARGET_OBJECTS:${T}_user>)
    target_link_options(${T} PRIVATE
        ${CPU_FLAGS}
        -T${LD}
        -nostartfiles
        -Wl,--gc-sections,--print-memory-usage
        -Wl,-Map,${T}.map
        --specs=nano.specs
        --specs=nosys.specs)
    set_target_properties(${T} PROPERTIES SUFFIX ".elf" LINK_DEPENDS ${LD})

    add_custom_command(TARGET ${T} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${T}> ${T}.hex
        COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${T}>
        BYPRODUCTS ${T}.hex
        VERBATIM)
endfunction()

h417_core(V3F)
h417_core(V5F)

# MRS 1.92 держит отладку проекта в <проект>.launch с программой obj/<проект>.elf — это ядро V3F.
add_custom_command(TARGET H417_V3F POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:H417_V3F> ${PROJECT_NAME}.elf
    BYPRODUCTS ${PROJECT_NAME}.elf
    VERBATIM)

# Склейка: hex V3F без записи конца файла + hex V5F; bin — тот же образ с адреса 0, пустоты 0xFF.
add_custom_command(
    OUTPUT H417_Merge.hex H417_Merge.bin
    COMMAND ${CMAKE_COMMAND} -DIN1=H417_V3F.hex -DIN2=H417_V5F.hex -DOUT=H417_Merge.hex
            -P ${CMAKE_SOURCE_DIR}/cmake/merge_hex.cmake
    COMMAND ${CMAKE_OBJCOPY} -I ihex -O binary --gap-fill 0xFF H417_Merge.hex H417_Merge.bin
    DEPENDS H417_V3F H417_V5F ${CMAKE_SOURCE_DIR}/cmake/merge_hex.cmake
    COMMENT "H417_Merge.hex / H417_Merge.bin"
    VERBATIM)
add_custom_target(H417_Merge ALL DEPENDS H417_Merge.hex H417_Merge.bin)
