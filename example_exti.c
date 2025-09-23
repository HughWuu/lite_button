#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "lite_button.h"
#include "lite_button_cfg.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "lite_button.h"

/* 模拟 实际定时器 封装成btn_timer_cb_t结构体所需格式 */
void app_button_creat(btn_timer_callback_cb_f cb)
{
    //g_button_timer_id = osal_timer_create(cb, OSAL_TIMER_PERIODIC, NULL, NULL);
}
void app_button_start(uint32_t ms)
{
    //osal_timer_start(g_button_timer_id, ms);
}
void app_button_stop(void)
{
    //osal_timer_stop(g_button_timer_id);
}

/* 模拟 GPIO_KEY_UP EXTI 中断服务函数 */
void GPIOA10_IRQHandler(void)
{
    lite_button_exti_trigger(KEY_UP);
}

/* 模拟 GPIO 读函数 */
btn_level_e gpio_key_up_read(void)
{
    //return hal_gpio_read(GPIO_KEY_UP);
}

btn_level_e gpio_key_down_read(void)
{
    //return hal_gpio_read(GPIO_KEY_DOWN);
}

btn_level_e gpio_key_ok_read(void)
{
    //return hal_gpio_read(GPIO_KEY_OK);
}

/* 单键回调 */
void key_up_callback(btn_evt_e evt, void *para)
{
    const char *name = "KEY_UP";
    switch(evt) {
        case BTN_EVT_PRESS:
            printf("%s: PRESS\n", name);
            break;
        case BTN_EVT_RELEASE:
            printf("%s: RELEASE\n", name);
            break;
        case BTN_EVT_DOUBLE:
            printf("%s: DOUBLE\n", name);
            break;
        case BTN_EVT_TRIPLE:
            printf("%s: TRIPLE\n", name);
            break;
        case BTN_EVT_LONG:
            printf("%s: LONG\n", name);
            break;
        default: break;
    }
}

void key_down_callback(btn_evt_e evt, void *para)
{
    const char *name = "KEY_DOWN";
    switch(evt) {
        case BTN_EVT_PRESS:
            printf("%s: PRESS\n", name);
            break;
        case BTN_EVT_RELEASE:
            printf("%s: RELEASE\n", name);
            break;
        case BTN_EVT_DOUBLE:
            printf("%s: DOUBLE\n", name);
            break;
        case BTN_EVT_TRIPLE:
            printf("%s: TRIPLE\n", name);
            break;
        case BTN_EVT_LONG:
            printf("%s: LONG\n", name);
            break;
        default: break;
    }
}

void key_ok_callback(btn_evt_e evt, void *para)
{
    const char *name = "KEY_OK";
    switch(evt) {
        case BTN_EVT_PRESS:
            printf("%s: PRESS\n", name);
            break;
        case BTN_EVT_RELEASE:
            printf("%s: RELEASE\n", name);
            break;
        case BTN_EVT_DOUBLE:
            printf("%s: DOUBLE\n", name);
            break;
        case BTN_EVT_TRIPLE:
            printf("%s: TRIPLE\n", name);
            break;
        case BTN_EVT_LONG:
            printf("%s: LONG\n", name);
            break;
        default: break;
    }
}

/* 组合键回调 */
void combo_callback(key_combo_id_e combo_id, void *para)
{
    switch(combo_id) {
        case KEY_COMBO_COPY:
            printf("Combo COPY triggered\n");
            break;
        case KEY_COMBO_PASTE:
            printf("Combo PASTE triggered\n");
            break;
        case KEY_COMBO_SCREENSHOT:
            printf("Combo SCREENSHOT triggered\n");
            break;
        default: break;
    }
}

int main(void)
{
    /* 按键配置：长按 5s，重复触发 2s */
    btn_cfg_t cfg1 = {
        .longpress_ms = (5 * 1000), // 无长按功能设置0即可
        .longpress_repeat_ms = (2 * 1000),
    };

    btn_cfg_t cfg2 = {
        .longpress_ms = (1 * 1000),
        .longpress_repeat_ms = 0,
    };

    btn_cfg_t cfg3 = {
        .longpress_ms = 0,
        .longpress_repeat_ms = 0,
    };

    /* 初始化单键 */
    lite_button_init(KEY_UP, gpio_key_up_read, &cfg1, key_up_callback, NULL);
    lite_button_init(KEY_DOWN, gpio_key_down_read, &cfg2, key_down_callback, NULL);
    lite_button_init(KEY_OK, gpio_key_ok_read, &cfg3, key_ok_callback, NULL);

    /* 注册组合键：顺序按 UP -> DOWN -> COPY */
    btn_combo_cfg_t combo1_cfg = {
        .keys = {KEY_UP, KEY_DOWN},
        .num = BTN_DOUBLE_KEY_CNT,
        .type = BTN_COMBO_SEQUENTIAL
    };
    lite_button_register_combos(KEY_COMBO_COPY, &combo1_cfg, combo_callback, NULL);

    /* 注册组合键：同时按 UP + DOWN + OK -> SCREENSHOT */
    btn_combo_cfg_t combo2_cfg = {
        .keys = {KEY_UP, KEY_DOWN, KEY_OK},
        .num = BTN_TRIPLE_KEY_CNT,
        .type = BTN_COMBO_SIMULTANEOUS
    };
    lite_button_register_combos(KEY_COMBO_SCREENSHOT, &combo2_cfg, combo_callback, NULL);

    /* 与轮询相比，需要多注册定时器回调这一步骤 */
    btn_timer_cb_t cb;
    cb.creat = app_button_creat;
    cb.start = app_button_start;
    cb.stop = app_button_stop;
    lite_button_register_timer(&cb);

    /* 模拟主循环 */
    while(1) {
    }

    return 0;
}
