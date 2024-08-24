/**
 * @brief bit[23:0] 0xFFFFFF = 16,777,215‬
 *
 * SysTick->CTRL 
 * bit[16]R COUNTFLAG 如果在上次读取本寄存器后，SysTick 已经计到
 *                    了 0，则该位为 1。如果读取该位，该位将自动清零
 * bit[ 2]R/W CLKSOURCE 0=外部时钟源(STCLK) 
 *                      1=内核时钟(FCLK)
 * bit[ 1]R/W TICKINT   0=数到 0 时无动作
 *                      1=SysTick 倒数计数到 0 时产生 SysTick 异常请求
 * bit[ 0]R/W ENABLE    SysTick 定时器的使能位
 *
 * SysTick->LOAD
 * bit[23:0]R/W RELOAD 当倒数计数至零时，将被重装载的值
 *
 * SysTick->VAL
 * bit[23:0]R/Wc CURRENT 读取时返回当前倒计数的值，写它则使之清零，
 *                       同时还会清除在SysTick控制及状态寄存器中的
 *                       COUNTFLAG 标志
 * Systick->CALIB
 * bit[31]R NOREF 0=外部参考时钟可用
 *                1=没有外部参考时钟（STCLK 不可用）
 * bit[30]R SKEW  0=校准值是准确的 10ms
 *                1=校准值不是准确的 10ms
 * bit[23:0]R/W TENMS 在 10ms 的间隔中倒计数的格数。芯片设计者应
 *                    该通过 Cortex-M3 的输入信号提供该数值。若该
 *                    值读回零，则表示无法使用校准功能
 */

#include "delay.h"
//#include "stm32f10x.h"
#include "stm32f4xx.h"

static volatile uint32_t ticks_per_us;
static volatile uint32_t ticks_per_ms;

static void systick_tick(volatile uint32_t ticks)
{
    SysTick->LOAD = ticks;
    SysTick->VAL = 0UL;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    /* systick 倒计数为0时 COUNTFLAG位置一 */
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0UL; // 重置倒计时值 清除 COUNTFLAG 位
}

static void delay(volatile uint32_t ticks)
{
    uint32_t repeat = ticks / SysTick_LOAD_RELOAD_Msk;
    uint32_t remain = ticks % SysTick_LOAD_RELOAD_Msk;
    
    if (ticks <= SysTick_LOAD_RELOAD_Msk) {
        systick_tick(ticks);
    } else {
        while (repeat) {
            systick_tick(SysTick_LOAD_RELOAD_Msk);
            repeat--;
        }
        if (remain) {
            systick_tick(remain);
        }
    }
}

/**
 * @brief 微秒级延时函数
 *
 * @param n_us 延时n微秒
 * @retval none
 */
void delay_us(volatile uint32_t n_us)
{
    if (!ticks_per_us)
        return;
    delay((n_us * ticks_per_us) - 1UL);
}

/**
 * @brief 毫秒级延时函数
 *
 * @param n_ms 延时n毫秒
 * @retval none
 */
void delay_ms(volatile uint32_t n_ms)
{
    if (!ticks_per_ms)
        return;
    delay((n_ms * ticks_per_ms) - 1UL);
}

/**
 * @brief system delay function init
 *
 * @retval none
 */
void delay_init(void)
{
    /* SysTick ClockSource = HCLK / 8 */
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
    
    ticks_per_us = SystemCoreClock / 8UL / 1000000UL;
    ticks_per_ms = ticks_per_us * 1000UL;
}

