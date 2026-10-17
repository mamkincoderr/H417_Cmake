/* H417_Cmake, ядро V5F (ведомое): запускается ядром V3F с адреса 0x10000, мигает LED2.
 * Автор: Дмитрий (GitHub: mamkincoderr, https://github.com/mamkincoderr)
 * Telegram: https://t.me/oDeXteRo
 *
 * Основа — пример WCH EVT GPIO_Toggle (openwch/ch32h417, EVT V1.6).
 * USART1 настроило ядро V3F, здесь он не трогается. Печать только до сигнала «готов»:
 * в это время V3F ждёт и сам не печатает.
 */
#include "debug.h"
#include "board.h"

volatile uint32_t v5f_ticks;    /* счётчик для проверки в отладчике */

int main(void)
{
    SystemAndCoreClockUpdate();
    Delay_Init();
    printf("V5F: core %u Hz\r\n", (unsigned)SystemCoreClock);

    /* GPIOE до сигнала: V3F настраивает свой вывод того же порта только после него. */
    board_led_init(LED2_PIN);

    /* Захват и освобождение HSEM ставят ведущему ядру флаг: V5F запущен. */
    HSEM_FastTake(HSEM_V5F_READY);
    HSEM_ReleaseOneSem(HSEM_V5F_READY, 0);

    while (1) {
        board_led_set(LED2_PIN, v5f_ticks & 1);
        v5f_ticks++;
        Delay_Ms(100);
    }
}
