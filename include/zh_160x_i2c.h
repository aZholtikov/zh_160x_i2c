/**
 * @file zh_160x_i2c.h
 */

#pragma once

#include "zh_pcf8574.h"

#define ZH_LCD_16X2 1 /*!< LCD size 16x2. */
#define ZH_LCD_16X4 0 /*!< LCD size 16x4. */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initializes the LCD 160X.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] size LCD size (ZH_LCD_16X2 or ZH_LCD_16X4).
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_160x_init(zh_pcf8574_handle_t *handle, bool size);

    /**
     * @brief Clears the LCD screen.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_160x_lcd_clear(zh_pcf8574_handle_t *handle);

    /**
     * @brief Sets the cursor to a specific position on the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] row The row number.
     * @param[in] col The column number (0–15).
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_160x_set_cursor(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t col);

    /**
     * @brief Prints a string to the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] str Pointer to the string to be displayed.
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_160x_print_char(zh_pcf8574_handle_t *handle, const char *str);

    /**
     * @brief Prints an integer to the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] num The integer to be displayed.
     *
     * @return ESP_OK if success or an error code otherwise..
     */
    esp_err_t zh_160x_print_int(zh_pcf8574_handle_t *handle, int num);

    /**
     * @brief Prints a floating-point number to the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] num The floating-point number to be displayed.
     * @param[in] precision The number of decimal places to display.
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_160x_print_float(zh_pcf8574_handle_t *handle, float num, uint8_t precision);

    /**
     * @brief Displays a progress bar on a specific row of the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] row The row number.
     * @param[in] progress The progress percentage (0–100).
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_160x_print_progress_bar(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t progress);

    /**
     * @brief Clears a specific row on the LCD.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] row The row number.
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_160x_clear_row(zh_pcf8574_handle_t *handle, uint8_t row);

    /**
     * @brief Enable the cursor.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     * @param[in] blink Blink mode.
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_160x_on_cursor(zh_pcf8574_handle_t *handle, bool blink);

    /**
     * @brief Disable the cursor.
     *
     * @param[in] handle Pointer to unique PCF8574 handle.
     *
     * @return ESP_OK if success or an error code otherwise.
     */
    esp_err_t zh_160x_off_cursor(zh_pcf8574_handle_t *handle);

#ifdef __cplusplus
}
#endif