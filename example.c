#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "lite_button.h"
#include "lite_button_cfg.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "lite_button.h"

bool g_20ms_flag = false;

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

    /* 模拟主循环 */
    while(1) {
        if (g_20ms_flag) { // 每 20ms 调用一次
            lite_button_poll_handle(); // BTN_POLL_PERIOD_MS配置需要与轮询周期一致
        }
    }

    return 0;
}
