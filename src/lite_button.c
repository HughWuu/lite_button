/**
 * @file    lite_button.c
 * @brief   Implementation of lightweight button handling library.
 *
 * Contains internal state machines and logic for:
 *   - Debouncing
 *   - Press/release detection
 *   - Long press and repeat press(option)
 *   - Multi-click(option)
 *   - Combo keys(option)
 *
 * @author  HughWu
 * @date    2025-08-21
 * @version 1.0
 *
 * @note
 *   - Requires lite_button_cfg.h for configuration.
 *   - Only lite_button.h should be included by application code.
 */

#include "lite_button.h"

static size_t btn_tmr_tick = 0;
static size_t g_btn_combo_num = 0;
static uint32_t g_btn_press_mask = 0;
static btn_t g_btn_list[BTN_NUM] = {0};
static btn_combo_t g_btn_combo_list[BTN_COMBO_NUM] = {0};

#if BTN_COMBO_FUN_ENABLE
static size_t lite_button_combo_tick_diff(key_id_e *keys, btn_combo_num_e num)
{
    size_t t0 = g_btn_list[keys[0]].prs_tick;
    size_t t1 = g_btn_list[keys[1]].prs_tick;
    size_t t2 = 0;

    if (num == BTN_DOUBLE_KEY_CNT) {
        return ABS_DIFF(t0, t1);
    } else if (num == BTN_TRIPLE_KEY_CNT) {
        t2 = g_btn_list[keys[2]].prs_tick;
        return MAX3_DIFF(t0, t1, t2);
    }

    return -1;
}

static void lite_button_combo_handle(void)
{
    btn_combo_t *combo = NULL;

    for (size_t i = 0; i < BTN_COMBO_NUM; i++) {
        combo = &g_btn_combo_list[i];
        if (combo->cb == NULL || combo->cfg.num == 0) continue;

        if (g_btn_press_mask != combo->keys_mask) continue;
        g_btn_press_mask &= ~combo->keys_mask;
        if (combo->cfg.type == BTN_COMBO_SIMULTANEOUS) {
            if (lite_button_combo_tick_diff(combo->cfg.keys, combo->cfg.num) <= BTN_COMBO_GAP_THR) {
                combo->cb(i, combo->para);
            }
        } else if (combo->cfg.type == BTN_COMBO_SEQUENTIAL) {
            for (size_t k = 0; k < combo->cfg.num - 1; k++) {
                if (g_btn_list[combo->cfg.keys[k]].prs_tick >=
                    g_btn_list[combo->cfg.keys[k + 1]].prs_tick) {
                    return;
                }
            }
            combo->cb(i, combo->para);
        }
    }
}
#endif

#if BTN_MULTICLICK_FUN_ENABLE
static void lite_button_multi_click_handle(btn_t *btn)
{
    uint32_t interval = (btn_tmr_tick > btn->rel_tick)?
                        (btn_tmr_tick - btn->rel_tick) : (0xFFFFFFFF - btn->rel_tick + btn_tmr_tick);

    if(interval <= BTN_MULTI_GAP_THR) {
        btn->click_cnt++;
    } else {
        btn->click_cnt = BTN_SINGLE_CLICK;
    }

    if(btn->click_cnt == BTN_SINGLE_CLICK) {
        btn->cb(BTN_EVT_RELEASE, btn->cb_para);
    } else if(btn->click_cnt == BTN_DOUBLE_CLICK) {
        btn->cb(BTN_EVT_DOUBLE, btn->cb_para);
    } else if(btn->click_cnt == BTN_TRIPLE_CLICK) {
        btn->cb(BTN_EVT_TRIPLE, btn->cb_para);
    }
}
#endif

#if BTN_LONGPRESS_FUN_ENABLE
static void lite_button_long_press_handle(btn_t *btn)
{
    if (btn->lp_cnt == 0) return;
    if (btn->state == BTN_IDLE_LEVEL) return;

    btn->lp_cnt--;
    if (btn->lp_cnt == 0) {
        btn->lp_cnt = btn->cfg.lp_rpt_thr;
        btn->cb(BTN_EVT_LONG, btn->cb_para);
    }
}
#endif

void lite_button_poll_handle(void)
{
    btn_t *btn = NULL;
    btn_level_e cur_lv = BTN_IDLE_LEVEL;

    btn_tmr_tick++;
    for(size_t i = 0; i < BTN_NUM; i++) {
        btn = &g_btn_list[i];
        if (btn->cb == NULL || btn->gpio_cb == NULL) continue;

        cur_lv = btn->gpio_cb();
        // debounce
        if(btn->state == cur_lv) {
            btn->deb_cnt = 0;
        } else {
            btn->deb_cnt++;
            if(btn->deb_cnt > BTN_DEBOUNCE_THR) {
                // switch state
                btn->state = cur_lv;
                btn->deb_cnt = 0;
                btn->lp_cnt = btn->cfg.lp_thr;

                // button press
                if(btn->state == BTN_ACTIVE_LEVEL) {
                    g_btn_press_mask |= BIT(i);
                    btn->prs_tick = btn_tmr_tick;
                    btn->cb(BTN_EVT_PRESS, btn->cb_para);
                }
                // button release
                if(btn->state != BTN_ACTIVE_LEVEL) {
                    g_btn_press_mask &= ~BIT(i);
                    btn->rel_tick = btn_tmr_tick;
#if BTN_MULTICLICK_FUN_ENABLE
                    lite_button_multi_click_handle(btn);
#else
                    btn->cb(BTN_EVT_RELEASE, btn->cb_para);
#endif
                }
            }
        }

        // long press
#if BTN_LONGPRESS_FUN_ENABLE
        lite_button_long_press_handle(btn);
#endif
    }

    // combo
#if BTN_COMBO_FUN_ENABLE
    lite_button_combo_handle();
#endif
}

void lite_button_register_combos(key_combo_id_e id, const btn_combo_cfg_t *cfg,
                                 btn_combo_cb_f cb, void *para)
{
    if (id >= BTN_COMBO_NUM) return;

    g_btn_combo_list[id].cb = cb;
    g_btn_combo_list[id].para = para;
    g_btn_combo_list[id].keys_mask = 0;

    memcpy(&g_btn_combo_list[id].cfg, cfg, sizeof(btn_combo_cfg_t));
    for (size_t i = 0; i < cfg->num; i++) {
        if (cfg->keys[i] < BTN_NUM) {
            g_btn_combo_list[id].keys_mask |= BIT(cfg->keys[i]);
        }
    }
}

void lite_button_init(key_id_e id, btn_gpio_lv_f gpio_cb,
                      const btn_cfg_t *cfg, btn_cb_f cb, void *para)
{
    if (id >= BTN_NUM) return;

    g_btn_list[id].gpio_cb = gpio_cb;
    g_btn_list[id].cb = cb;
    g_btn_list[id].cb_para = para;

    g_btn_list[id].cfg.lp_thr = cfg->longpress_ms / BTN_POLL_PERIOD_MS;
    g_btn_list[id].cfg.lp_rpt_thr = cfg->longpress_repeat_ms / BTN_POLL_PERIOD_MS;

    g_btn_list[id].state = BTN_IDLE_LEVEL;
    g_btn_list[id].deb_cnt = 0;
    g_btn_list[id].lp_cnt = 0;
    g_btn_list[id].click_cnt = 0;
}
