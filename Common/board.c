/* Светодиоды платы CH32H417QEU6-R0. Код общий для обоих ядер.
 * Автор: Дмитрий (GitHub: mamkincoderr, https://github.com/mamkincoderr)
 * Telegram: https://t.me/oDeXteRo
 *
 * Оба ядра работают с одним портом GPIOE. GPIO_Init переписывает CFGLR целиком (чтение-изменение-запись),
 * поэтому выводы настраиваются по очереди: V5F до сигнала «готов», V3F после него.
 * Переключение — через BSHR/BCR, оно атомарно и ядрам не мешает.
 */
#include "board.h"

/* Вывод светодиода: двухтактный выход, светодиод погашен (на выводе 1). */
void board_led_init(uint16_t pin)
{
    GPIO_InitTypeDef gpio = {0};

    RCC_HB2PeriphClockCmd(LED_PORT_CLK, ENABLE);
    GPIO_SetBits(LED_PORT, pin);                /* погашен */
    gpio.GPIO_Pin = pin;
    gpio.GPIO_Speed = GPIO_Speed_Low;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED_PORT, &gpio);
}

/* Светодиод горит при 0 на выводе: анод через 1 кОм к VDDIO. */
void board_led_set(uint16_t pin, int on)
{
    if (on) GPIO_ResetBits(LED_PORT, pin);
    else    GPIO_SetBits(LED_PORT, pin);
}
