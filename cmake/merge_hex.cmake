# Склейка двух Intel HEX: IN1 без записи конца файла (:00000001FF) + IN2 целиком -> OUT.
# Автор: Дмитрий (GitHub: mamkincoderr), Telegram: https://t.me/oDeXteRo
#
# Запуск: cmake -DIN1=a.hex -DIN2=b.hex -DOUT=c.hex -P merge_hex.cmake
# Образы ядер не пересекаются (V3F с 0x00000, V5F с 0x10000), поэтому достаточно склеить записи.
# NO_HEX_CONVERSION обязателен: иначе file(STRINGS) сам переводит Intel HEX в двоичные данные.
file(STRINGS ${IN1} _a NO_HEX_CONVERSION)
file(READ ${IN2} _b)
set(_out "")
foreach(_l IN LISTS _a)
    if(NOT _l STREQUAL ":00000001FF")
        string(APPEND _out "${_l}\n")
    endif()
endforeach()
file(WRITE ${OUT} "${_out}${_b}")
