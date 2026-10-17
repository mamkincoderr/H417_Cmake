/* Плата CH32H417QEU6-R0-1v1 (оценочная плата WCH): выводы и светодиоды.
 * Автор: Дмитрий (GitHub: mamkincoderr, https://github.com/mamkincoderr)
 * Telegram: https://t.me/oDeXteRo
 *
 * Цепи — по CH32H417QEU6-R0.PcbDoc (docs/board_CH32H417QEU6-R0).
 *   LED1/LED2: анод через 1 кОм к VDDIO, катод на гребёнку P8.29/P8.30 -> горят при 0.
 *              К МК — перемычками P8.27 (PE2) - P8.29 и P8.28 (PE3) - P8.30.
 *   KEY (S2):  P5.2, подтяжка 10 кОм к 3V3.
 *   printf:    USART1 TX = PA9 (P5.32).
 *   Отладка:   P3: PB9 = SWDIO (конт. 5), PB8 = SWCLK (конт. 7).
 */
#ifndef BOARD_H
#define BOARD_H

#include "ch32h417.h"

#define LED_PORT        GPIOE
#define LED_PORT_CLK    RCC_HB2Periph_GPIOE
#define LED1_PIN        GPIO_Pin_2      /* V3F */
#define LED2_PIN        GPIO_Pin_3      /* V5F */

/* HSEM, по которому V5F сообщает ведущему ядру, что запустился. */
#define HSEM_V5F_READY  HSEM_ID0

void board_led_init(uint16_t pin);
void board_led_set(uint16_t pin, int on);

#endif
