/**
 * @file zh_160x_i2c.h
 *
 * @brief Driver for 16x2 and 16x4 LCD displays via I2C PCF8574 expander.
 *
 * Provides a high-level API for controlling HD44780-compatible LCD modules
 * connected through a PCF8574 I/O expander. Supports text output, cursor
 * control, and progress bar display.
 *
 * Key features:
 * - 4-bit interface mode for efficient data transfer
 * - Support for 16x2 and 16x4 LCD configurations
 * - Text, integer, and floating-point number printing
 * - Custom progress bar rendering
 * - Cursor visibility and blink control
 */

#pragma once

#include "zh_pcf8574.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Opaque handle for LCD 160x I2C display.
     *
     * Internal structure containing the PCF8574 expander handle and
     * current LCD size configuration.
     */
    typedef struct _zh_160x_i2c_handle_t zh_160x_i2c_handle_t;

    /**
     * @brief Supported LCD display sizes.
     */
    typedef enum
    {
        ZH_LCD_16X2 = 0, /*!< LCD size 16x2 */
        ZH_LCD_16X4,     /*!< LCD size 16x4 */
        ZH_LCD_NUM_MAX
    } zh_160x_i2c_lcd_size_t;

    /**
     * @brief Initializes the LCD 160x display.
     *
     * Allocates and configures the LCD handle, then sends the initialization
     * sequence to the LCD controller via the PCF8574 I2C expander.
     * Configures 4-bit mode and sets default display parameters.
     *
     * @param[in] expander Pointer to the PCF8574 expander handle (must not be NULL)
     * @param[out] handle Output pointer for the newly allocated LCD handle (must be NULL)
     * @param[in] size LCD size (ZH_LCD_16X2 or ZH_LCD_16X4)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if any parameter is NULL or size is invalid
     * @return ESP_ERR_INVALID_STATE if the handle is already initialized
     * @return ESP_ERR_NO_MEM if memory allocation fails
     * @return ESP_FAIL if LCD initialization sequence fails
     */
    esp_err_t zh_160x_init(zh_pcf8574_handle_t **expander, zh_160x_i2c_handle_t **handle, zh_160x_i2c_lcd_size_t size);

    /**
     * @brief Deinitializes the LCD 160x display and frees the handle.
     *
     * Releases the allocated memory for the LCD handle and sets the output
     * pointer to NULL to prevent use-after-free.
     *
     * @param[in,out] handle Pointer to the LCD handle (must not be NULL)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle is NULL
     */
    esp_err_t zh_160x_deinit(zh_160x_i2c_handle_t **handle);

    /**
     * @brief Clears the LCD screen and moves cursor to home position.
     *
     * Sends the LCD clear command (0x01) to erase all displayed text
     * and reset the cursor to the upper-left corner (row 0, column 0).
     *
     * @param[in] handle Pointer to the LCD handle (must not be NULL)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle is NULL
     * @return ESP_FAIL if sending the command fails
     */
    esp_err_t zh_160x_lcd_clear(zh_160x_i2c_handle_t **handle);

    /**
     * @brief Sets the cursor to a specific position on the LCD.
     *
     * Moves the cursor to the specified row and column.
     * For 16x2 displays: rows 0–1, columns 0–15.
     * For 16x4 displays: rows 0–3, columns 0–15.
     *
     * @param[in] handle Pointer to the LCD handle (must not be NULL)
     * @param[in] row Row number (0 to 1 for 16x2, 0 to 3 for 16x4)
     * @param[in] col Column number (0–15)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle is NULL or coordinates are out of range
     * @return ESP_FAIL if sending the command fails
     *
     * @note Row address mapping: row 0 = 0x00, row 1 = 0x40,
     *       row 2 = 0x10, row 3 = 0x50.
     */
    esp_err_t zh_160x_set_cursor(zh_160x_i2c_handle_t **handle, uint8_t row, uint8_t col);

    /**
     * @brief Prints a string to the LCD at the current cursor position.
     *
     * Transmits each character of the null-terminated string to the LCD
     * in 4-bit mode. The cursor advances automatically after each character.
     *
     * @param[in] handle Pointer to the LCD handle (must not be NULL)
     * @param[in] str Null-terminated string to display (must not be NULL)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle or str is NULL
     * @return ESP_FAIL if any character transmission fails
     */
    esp_err_t zh_160x_print_char(zh_160x_i2c_handle_t **handle, const char *str);

    /**
     * @brief Prints an integer to the LCD at the current cursor position.
     *
     * Converts the integer to a decimal string and displays it.
     * Supports negative numbers (maximum 11 digits plus sign).
     *
     * @param[in] handle Pointer to the LCD handle (must not be NULL)
     * @param[in] num Integer value to display
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle is NULL
     * @return ESP_FAIL if string conversion or printing fails
     */
    esp_err_t zh_160x_print_int(zh_160x_i2c_handle_t **handle, int num);

    /**
     * @brief Prints a floating-point number to the LCD at the current cursor position.
     *
     * Converts the floating-point value to a string with the specified
     * decimal precision and displays it.
     *
     * @param[in] handle Pointer to the LCD handle (must not be NULL)
     * @param[in] num Floating-point value to display
     * @param[in] precision Number of decimal places (0–6 recommended)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle is NULL
     * @return ESP_FAIL if string conversion or printing fails
     */
    esp_err_t zh_160x_print_float(zh_160x_i2c_handle_t **handle, float num, uint8_t precision);

    /**
     * @brief Displays a progress bar on a specific row of the LCD.
     *
     * Renders a 16-block progress bar using custom character (0xFF).
     * Filled blocks represent the progress percentage, empty blocks show remaining.
     *
     * @param[in] handle Pointer to the LCD handle (must not be NULL)
     * @param[in] row Row number (0 to 1 for 16x2, 0 to 3 for 16x4)
     * @param[in] progress Progress percentage (0–100)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle is NULL, row is out of range,
     *         or progress exceeds 100
     * @return ESP_FAIL if cursor positioning or character printing fails
     */
    esp_err_t zh_160x_print_progress_bar(zh_160x_i2c_handle_t **handle, uint8_t row, uint8_t progress);

    /**
     * @brief Clears a specific row on the LCD.
     *
     * Moves the cursor to the beginning of the specified row and fills
     * all 16 positions with spaces to erase existing content.
     * Leaves the cursor at the start of the cleared row.
     *
     * @param[in] handle Pointer to the LCD handle (must not be NULL)
     * @param[in] row Row number (0 to 1 for 16x2, 0 to 3 for 16x4)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle is NULL or row is out of range
     * @return ESP_FAIL if cursor positioning or character printing fails
     */
    esp_err_t zh_160x_clear_row(zh_160x_i2c_handle_t **handle, uint8_t row);

    /**
     * @brief Enables the cursor with optional blink mode.
     *
     * Turns on the cursor and optionally enables blinking.
     * Command 0x0F enables cursor with blink, 0x0E enables cursor only.
     *
     * @param[in] handle Pointer to the LCD handle (must not be NULL)
     * @param[in] blink If true, enables blinking cursor; if false, solid cursor
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle is NULL
     * @return ESP_FAIL if sending the command fails
     */
    esp_err_t zh_160x_on_cursor(zh_160x_i2c_handle_t **handle, bool blink);

    /**
     * @brief Hides the cursor and disables blinking.
     *
     * Sends command 0x0C to turn off both cursor and cursor blink.
     *
     * @param[in] handle Pointer to the LCD handle (must not be NULL)
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG if handle is NULL
     * @return ESP_FAIL if sending the command fails
     */
    esp_err_t zh_160x_off_cursor(zh_160x_i2c_handle_t **handle);

#ifdef __cplusplus
}
#endif