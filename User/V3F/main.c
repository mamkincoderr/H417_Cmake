/* H417_Cmake, ядро V3F (ведущее): стартует первым, настраивает USART1, будит V5F, мигает LED1.
 * Автор: Дмитрий (GitHub: mamkincoderr, https://github.com/mamkincoderr)
 * Telegram: https://t.me/oDeXteRo
 *
 * Основа — пример WCH EVT GPIO_Toggle (openwch/ch32h417, EVT V1.6). В отличие от него V5F ждём опросом
 * флага HSEM, без режима STOP: в STOP отладчик к чипу не подключается.
 */
#include "debug.h"
#include "board.h"

volatile uint32_t v3f_ticks;    /* счётчик для проверки в отладчике */

/* Будит V5F с адреса Core_V5F_StartAddr (0x10000) и ждёт до 2 с, пока он отпустит HSEM.
 * Возвращает 1, если V5F ответил. */
static int start_v5f(void)
{
    HSEM_ClearFlag(HSEM_V5F_READY);
    HSEM_ITConfig(HSEM_V5F_READY, ENABLE);
    NVIC_WakeUp_V5F(Core_V5F_StartAddr);

    for (uint32_t ms = 0; ms < 2000; ms++) {
        if (HSEM_GetFlagStatus(HSEM_V5F_READY) == SET) {
            HSEM_ClearFlag(HSEM_V5F_READY);
            return 1;
        }
        Delay_Ms(1);
    }
    return 0;
}

int main(void)
{
    SystemInit();                   /* тактирование всего чипа настраивает только V3F */
    SystemAndCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);
    printf("\r\nH417_Cmake V3F: SystemClk %u Hz, V3F core %u Hz\r\n",
           (unsigned)SystemClock, (unsigned)SystemCoreClock);

    if (start_v5f()) printf("V3F: V5F is running\r\n");
    else             printf("V3F: no answer from V5F in 2 s\r\n");

    board_led_init(LED1_PIN);
    while (1) {
        board_led_set(LED1_PIN, v3f_ticks & 1);
        if ((v3f_ticks % 8) == 0) printf("V3F tick %u\r\n", (unsigned)v3f_ticks);
        v3f_ticks++;
        Delay_Ms(250);
    }
}
