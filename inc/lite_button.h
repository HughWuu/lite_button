/**
 * @file    lite_button.h
 * @brief   Lightweight button handling library (public API).
 *
 * Provides simple button handling for embedded systems:
 *   - Single press / release events
 *   - Debounce filtering
 *   - Multi-click detection(option)
 *   - Long press and repeat press(option)
 *   - Combo key support (simultaneous & sequential)(option)
 *
 * @author  HughWu
 * @date    2025-08-16
 * @version 1.0
 */

#ifndef __LITE_BUTTON_H__
#define __LITE_BUTTON_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "lite_button_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BTN_NUM              KEY_MAX
#define BTN_COMBO_NUM        KEY_COMBO_MAX
#define BTN_DEBOUNCE_THR     (BTN_DEBOUNCE_MS / BTN_POLL_PERIOD_MS)
#define BTN_MULTI_GAP_THR    (BTN_MULTI_GAP_MS / BTN_POLL_PERIOD_MS)
#define BTN_COMBO_GAP_THR    (BTN_COMBO_GAP_MS / BTN_POLL_PERIOD_MS)

#define BTN_COMBO_KEY_NUM    (3)

#if (BTN_ACTIVE_LEVEL == BTN_LEVEL_LOW)
    #define BTN_IDLE_LEVEL     BTN_LEVEL_HIGH
#else
    #define BTN_IDLE_LEVEL     BTN_LEVEL_LOW
#endif

#define BIT(n) (1U << (n))
#define ABS_DIFF(a, b)   (( (a) > (b) ) ? ((a) - (b)) : ((b) - (a)))
#define MAX3_DIFF(a, b, c)   \
        ( ((ABS_DIFF(a, b)) > (ABS_DIFF(a, c))) ? \
        ( ((ABS_DIFF(a, b)) > (ABS_DIFF(b, c))) ? ABS_DIFF(a, b) : ABS_DIFF(b, c) ) : \
        ( ((ABS_DIFF(a, c)) > (ABS_DIFF(b, c))) ? ABS_DIFF(a, c) : ABS_DIFF(b, c) ) )
#define HAS_MULTI_BITS(x)   (((x) & ((x) - 1)) != 0)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define GET_INTERVAL(cur, prev) \
        ((cur) >= (prev) ? ((cur) - (prev)) : (SIZE_MAX - (prev) + (cur)))

typedef enum {
    BTN_LEVEL_LOW = 0,
    BTN_LEVEL_HIGH = 1,
} btn_level_e;

typedef enum {
    BTN_EVT_NONE = 0,
    BTN_EVT_PRESS,
    BTN_EVT_RELEASE,
    BTN_EVT_LONG,
    BTN_EVT_DOUBLE,
    BTN_EVT_TRIPLE,
    BTN_EVT_COMBO,
} btn_evt_e;

typedef btn_level_e (*btn_gpio_lv_f)(void);
typedef void (*btn_cb_f)(btn_evt_e evt, void *user);
typedef void (*btn_combo_cb_f)(key_combo_id_e, void *para);

typedef void (*btn_timer_callback_cb_f)(void);
typedef void (*btn_timer_creat_cb_f)(btn_timer_callback_cb_f cb);
typedef void (*btn_timer_start_cb_f)(uint32_t ms);
typedef void (*btn_timer_stop_cb_f)(void);

typedef struct {
    btn_timer_creat_cb_f creat;
    btn_timer_start_cb_f start;
    btn_timer_stop_cb_f stop;
} btn_timer_cb_t;

typedef struct {
    btn_timer_cb_t cb;
    bool run_flag;
    uint32_t exti_tick;
} btn_timer_t;

typedef enum {
    BTN_COMBO_NONE = 0,
    BTN_COMBO_SIMULTANEOUS,
    BTN_COMBO_SEQUENTIAL,
} btn_combo_type_e;

typedef enum {
    BTN_DOUBLE_KEY_CNT = 2,
    BTN_TRIPLE_KEY_CNT = 3,
} btn_combo_num_e;

typedef struct {
    key_id_e keys[BTN_COMBO_KEY_NUM];
    btn_combo_num_e num;
    btn_combo_type_e type;
} btn_combo_cfg_t;

typedef struct {
    btn_combo_cb_f cb;
    void *para;
    btn_combo_cfg_t cfg;
    uint32_t keys_mask;
} btn_combo_t;

typedef enum {
    BTN_SINGLE_CLICK = 1,
    BTN_DOUBLE_CLICK = 2,
    BTN_TRIPLE_CLICK = 3,
} btn_click_e;

typedef struct {
    uint32_t longpress_ms;
    uint32_t longpress_repeat_ms;
} btn_cfg_t;

typedef struct {
    size_t lp_thr;
    size_t lp_rpt_thr;
} btn_inner_cfg_t;

typedef struct {
    btn_gpio_lv_f gpio_cb;
    btn_cb_f cb;
    void *cb_para;
    btn_inner_cfg_t cfg;

    size_t deb_cnt;
    size_t lp_cnt;
    size_t prs_tick;
    size_t rel_tick;
    size_t click_cnt;
    btn_level_e state;
} btn_dev_t;

/*==============================================================================
 * API functions
 *============================================================================*/

/**
 * @brief Initialize a button
 *
 * @param id   Button ID (from key_id_e)
 * @param gpio GPIO read function
 * @param cfg  User configuration
 * @param cb   Callback function
 * @param para User parameter passed to callback
 */
void lite_button_init(key_id_e id, btn_gpio_lv_f gpio_cb,
                      const btn_cfg_t *cfg, btn_cb_f cb, void *para);

#if BTN_COMBO_FUN_ENABLE
/**
 * @brief Register a combo key
 *
 * @param id   Combo key ID (from key_combo_id_e)
 * @param cfg  Combo key configuration
 * @param cb   Callback function
 * @param para User parameter passed to callback
 */
void lite_button_register_combos(key_combo_id_e id, const btn_combo_cfg_t *cfg, btn_combo_cb_f cb, void *para);
#endif

#if BTN_EXTI_FUN_ENABLE
/**
 * @brief Register timer
 *
 * @param cb   Timer callback function
 */
void lite_button_register_timer(btn_timer_cb_t *cb);

/**
 * @brief EXIT call
 *
 * @param cb   EXIT irq handle call function
 */
void lite_button_exti_trigger(key_id_e i);
#else 

/**
 * @brief Poll handler, should be called periodically
 */
void lite_button_poll_handle(void);
#endif

#ifdef __cplusplus
}
#endif

#endif // __LITE_BUTTON_H__

