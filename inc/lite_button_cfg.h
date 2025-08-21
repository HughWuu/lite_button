/**
 * @file    lite_button_cfg.h
 * @brief   Configuration for lite_button library.
 *
 * This file defines compile-time options such as:
 *   - Active level
 *   - Polling period
 *   - debounce/multi/combo intervals
 *   - Enable/disable optional features
 *   - User Keys 
 *
 * @note Modify this file to adapt the library to your project.
 */

#ifndef __LITE_BUTTON_CONFIG_H__
#define __LITE_BUTTON_CONFIG_H__

#include "lite_button.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Button feature configuration
 *============================================================================*/

/** Active logic level of button input */
#define BTN_ACTIVE_LEVEL     BTN_LEVEL_LOW

/** Button polling period in milliseconds */
#define BTN_POLL_PERIOD_MS   (20)

/** Button debounce time in milliseconds */
#define BTN_DEBOUNCE_MS      (20)
/** Maximum interval for multi-click detection (ms) */
#define BTN_MULTI_GAP_MS     (200)
/** Maximum interval between combo keys (ms) */
#define BTN_COMBO_GAP_MS     (150)

/** Feature enable flags (0)-Disable (1)-Enable*/
#define BTN_LONGPRESS_FUN_ENABLE     (1)
#define BTN_MULTICLICK_FUN_ENABLE    (1)
#define BTN_COMBO_FUN_ENABLE         (1)

/*==============================================================================
 * Key ID definitions
 *============================================================================*/

/**
 * @brief Single key IDs
 */
typedef enum {
    KEY_UP = 0,
    KEY_DOWN,
    KEY_OK,

    KEY_MAX,
    KEY_INVALID,
} key_id_e;

/**
 * @brief Key combo IDs
 */
typedef enum {
    KEY_COMBO_COPY = 0,
    KEY_COMBO_PASTE,
    KEY_COMBO_SCREENSHOT,

    KEY_COMBO_MAX,
    KEY_COMBO_INVALID,
} key_combo_id_e;

#ifdef __cplusplus
}
#endif

#endif // __LITE_BUTTON_CONFIG_H__
