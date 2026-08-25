# zh_160x_i2c - LCD 1602A/1604A via I2C (PCF8574) Component for ESP-IDF

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [API Reference](#api-reference)
- [Usage Examples](#usage-examples)
- [Technical Specifications](#technical-specifications)
- [Error Codes](#error-codes)
- [Connection](#connection)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

`zh_160x_i2c` is a lightweight ESP-IDF component for controlling LCD 1602A and 1604A displays via I2C interface using PCF8574(A) I/O expanders. This component simplifies the process of displaying text, numbers, and progress bars on character LCD modules by providing a simple and intuitive API.

The component is designed specifically for ESP32 microcontrollers and uses ESP-IDF's I2C master driver and FreeRTOS for optimal performance in embedded systems.

---

## Features

1. **[Simple API]**: Easy-to-use functions for common LCD operations
2. **[I2C Interface]**: Uses only 2 GPIO pins (SCL and SDA) via PCF8574 expander
3. **[Support for 1602A and 1604A]**: Supports both 16x2 and 16x4 LCD modules
4. **[Multiple LCD Support]**: Up to 16 LCD modules on one I2C bus
5. **[Text Display]**: Print strings, integers, and floating-point numbers
6. **[Cursor Control]**: Enable/disable cursor with optional blink
7. **[Progress Bar]**: Display progress bars on LCD
8. **[Row Operations]**: Clear specific rows
9. **[ESP-IDF Optimized]**: Uses ESP-IDF's I2C master driver and FreeRTOS

---

## Installation

Navigate to your project's components directory:

```bash
cd ../your_project/components
```

Clone the repository:

```bash
git clone https://github.com/aZholtikov/zh_160x_i2c
```

In your application, include the header:

```c
#include "zh_160x_i2c.h"
```

The component will be automatically built with your project.

### Required menuconfig Settings

For correct operation, enable the following settings in menuconfig:

```text
CONFIG_FREERTOS_HZ=1000
CONFIG_I2C_ISR_IRAM_SAFE
CONFIG_I2C_MASTER_ISR_HANDLER_IN_IRAM
```

---

## API Reference

### Data Types

#### zh_160x_i2c_handle_t

```c
typedef struct _zh_160x_i2c_handle_t zh_160x_i2c_handle_t;
```

Opaque handle for LCD 160x I2C display. Internal structure containing the PCF8574 expander handle and the current LCD size configuration.

---

#### zh_160x_i2c_lcd_size_t

```c
typedef enum
{
    ZH_LCD_16X2 = 0, /*!< LCD size 16x2 */
    ZH_LCD_16X4,     /*!< LCD size 16x4 */
    ZH_LCD_NUM_MAX
} zh_160x_i2c_lcd_size_t;
```

Enumeration of supported LCD display sizes.

| Value | Description |
|-------|-------------|
| `ZH_LCD_16X2` | LCD size 16x2 (2 rows of 16 characters) |
| `ZH_LCD_16X4` | LCD size 16x4 (4 rows of 16 characters) |

---

### zh_160x_init()

Initializes the LCD 160x display.

Allocates and configures the LCD handle, then sends the initialization sequence to the LCD controller via the PCF8574 I2C expander. Configures 4-bit mode and sets default display parameters.

**Parameters:**

- `expander` - Pointer to the PCF8574 expander handle. Must not be NULL
- `handle` - Output pointer for the newly allocated LCD handle. Must be NULL
- `size` - LCD size (ZH_LCD_16X2 or ZH_LCD_16X4)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL pointer or invalid size)
- `ESP_ERR_INVALID_STATE` - Handle already initialized
- `ESP_ERR_NO_MEM` - Memory allocation failure
- `ESP_FAIL` - LCD initialization sequence failure

**Example:**

```c
zh_pcf8574_handle_t *lcd_expander = NULL;
zh_160x_i2c_handle_t *lcd_handle = NULL;
zh_pcf8574_init_config_t config = ZH_PCF8574_INIT_CONFIG_DEFAULT();
config.i2c_handle = i2c_bus_handle;
config.i2c_address = 0x27;
esp_err_t err = zh_pcf8574_init(&config, &lcd_expander);
if (err != ESP_OK) {
    // Error handling
}
err = zh_160x_init(&lcd_expander, &lcd_handle, ZH_LCD_16X2);
if (err != ESP_OK) {
    // Error handling
}
```

**Note:**

The PCF8574 expander must be initialized before initializing the LCD.

---

### zh_160x_lcd_clear()

Clears the LCD screen and moves the cursor to the home position.

Sends the LCD clear command (0x01) to erase all displayed text and reset the cursor to the upper-left corner (row 0, column 0).

**Parameters:**

- `handle` - Pointer to the LCD handle. Must not be NULL

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - Command send failure

---

### zh_160x_set_cursor()

Sets the cursor to a specific position on the LCD.

Moves the cursor to the specified row and column.
For 16x2 displays: rows 0–1, columns 0–15.
For 16x4 displays: rows 0–3, columns 0–15.

**Parameters:**

- `handle` - Pointer to the LCD handle. Must not be NULL
- `row` - Row number (0–1 for 16x2, 0–3 for 16x4)
- `col` - Column number (0–15)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle or coordinates out of range)
- `ESP_FAIL` - Command send failure

**Note:**

Row address mapping: row 0 = 0x00, row 1 = 0x40, row 2 = 0x10, row 3 = 0x50.

---

### zh_160x_print_char()

Prints a string to the LCD at the current cursor position.

Transmits each character of the null-terminated string to the LCD in 4-bit mode. The cursor advances automatically after each character.

**Parameters:**

- `handle` - Pointer to the LCD handle. Must not be NULL
- `str` - Pointer to the null-terminated string to display. Must not be NULL

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle or str)
- `ESP_FAIL` - Character transmission failure

---

### zh_160x_print_int()

Prints an integer to the LCD at the current cursor position.

Converts the integer to a decimal string and displays it.
Supports negative numbers (maximum 11 digits plus sign).

**Parameters:**

- `handle` - Pointer to the LCD handle. Must not be NULL
- `num` - Integer value to display

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - String conversion or print failure

---

### zh_160x_print_float()

Prints a floating-point number to the LCD at the current cursor position.

Converts the floating-point value to a string with the specified decimal precision and displays it.

**Parameters:**

- `handle` - Pointer to the LCD handle. Must not be NULL
- `num` - Floating-point value to display
- `precision` - Number of decimal places (0–6 recommended)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - String conversion or print failure

---

### zh_160x_print_progress_bar()

Displays a progress bar on a specific row of the LCD.

Renders a 16-block progress bar using a custom character (0xFF).
Filled blocks represent the progress percentage, empty blocks show the remainder.

**Parameters:**

- `handle` - Pointer to the LCD handle. Must not be NULL
- `row` - Row number (0–1 for 16x2, 0–3 for 16x4)
- `progress` - Progress percentage (0–100)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle, row out of range, or progress > 100)
- `ESP_FAIL` - Cursor positioning or character print failure

---

### zh_160x_clear_row()

Clears a specific row on the LCD.

Moves the cursor to the beginning of the specified row and fills all 16 positions with spaces to erase existing content.
The cursor remains at the start of the cleared row.

**Parameters:**

- `handle` - Pointer to the LCD handle. Must not be NULL
- `row` - Row number (0–1 for 16x2, 0–3 for 16x4)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle or row out of range)
- `ESP_FAIL` - Cursor positioning or character print failure

---

### zh_160x_on_cursor()

Enables the cursor with optional blink mode.

Turns on the cursor and optionally enables blinking.
Command 0x0F enables cursor with blink, 0x0E enables cursor only.

**Parameters:**

- `handle` - Pointer to the LCD handle. Must not be NULL
- `blink` - If true, enables blinking cursor; if false, solid cursor

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - Command send failure

---

### zh_160x_off_cursor()

Hides the cursor and disables blinking.

Sends command 0x0C to turn off both cursor and cursor blink.

**Parameters:**

- `handle` - Pointer to the LCD handle (not NULL)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - Command send failure

---

## Usage Examples

### Basic Example: Single LCD Display

```c
#include "zh_160x_i2c.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_pcf8574_handle_t *lcd_expander = NULL;
zh_160x_i2c_handle_t *lcd_handle = NULL;

void app_main(void)
{
    esp_log_level_set("zh_160x_i2c", ESP_LOG_ERROR);
    esp_log_level_set("zh_pcf8574", ESP_LOG_ERROR);
    esp_log_level_set("zh_vector", ESP_LOG_ERROR);
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
    zh_pcf8574_init_config_t config = ZH_PCF8574_INIT_CONFIG_DEFAULT();
    config.i2c_handle = i2c_bus_handle;
    config.i2c_address = 0x27;
    esp_err_t err = zh_pcf8574_init(&config, &lcd_expander);
    if (err != ESP_OK) {
        return;
    }
    err = zh_160x_init(&lcd_expander, &lcd_handle, ZH_LCD_16X2);
    if (err != ESP_OK) {
        return;
    }
    // Display text
    zh_160x_set_cursor(&lcd_handle, 0, 0);
    zh_160x_print_char(&lcd_handle, "LCD 160X");
    zh_160x_set_cursor(&lcd_handle, 1, 0);
    zh_160x_print_char(&lcd_handle, "Hello World!");
    // Cleanup
    zh_160x_deinit(&lcd_handle);
}
```

### Example: Progress Bar

```c
#include "zh_160x_i2c.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_pcf8574_handle_t *lcd_expander = NULL;
zh_160x_i2c_handle_t *lcd_handle = NULL;

void app_main(void)
{
    // ... initialization code ...
    for (uint8_t i = 0; i <= 100; ++i)
    {
        zh_160x_set_cursor(&lcd_handle, 0, 10);
        zh_160x_print_int(&lcd_handle, i);
        zh_160x_print_char(&lcd_handle, "%");
        zh_160x_print_progress_bar(&lcd_handle, 1, i);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    // Cleanup
    zh_160x_deinit(&lcd_handle);
}
```

### Example: Displaying Various Data Types

```c
#include "zh_160x_i2c.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_pcf8574_handle_t *lcd_expander = NULL;
zh_160x_i2c_handle_t *lcd_handle = NULL;

void app_main(void)
{
    // ... initialization code ...
    // Display string
    zh_160x_set_cursor(&lcd_handle, 0, 0);
    zh_160x_print_char(&lcd_handle, "String: ");
    zh_160x_print_char(&lcd_handle, "Hello");
    // Display integer
    zh_160x_set_cursor(&lcd_handle, 1, 0);
    zh_160x_print_int(&lcd_handle, 42);
    // Display floating-point number
    zh_160x_set_cursor(&lcd_handle, 0, 0);
    zh_160x_print_float(&lcd_handle, 3.14159, 2);  // 3.14
    // Enable cursor
    zh_160x_on_cursor(&lcd_handle, true);  // Blinking cursor
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    // Disable cursor
    zh_160x_off_cursor(&lcd_handle);
    // Cleanup
    zh_160x_deinit(&lcd_handle);
}
```

### Example: Multiple LCDs on One Bus

```c
#include "zh_160x_i2c.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_pcf8574_handle_t *lcd1_expander = NULL;
zh_160x_i2c_handle_t *lcd1_handle = NULL;
zh_pcf8574_handle_t *lcd2_expander = NULL;
zh_160x_i2c_handle_t *lcd2_handle = NULL;

void app_main(void)
{
    esp_log_level_set("zh_160x_i2c", ESP_LOG_ERROR);
    esp_log_level_set("zh_pcf8574", ESP_LOG_ERROR);
    esp_log_level_set("zh_vector", ESP_LOG_ERROR);
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_PORT,
        .scl_io_num = GPIO_NUM_22,
        .sda_io_num = GPIO_NUM_21,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t i2c_bus_handle = NULL;
    i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
    // Initialize first LCD at address 0x27
    zh_pcf8574_init_config_t config = ZH_PCF8574_INIT_CONFIG_DEFAULT();
    config.i2c_handle = i2c_bus_handle;
    config.i2c_address = 0x27;
    zh_pcf8574_init(&config, &lcd1_expander);
    zh_160x_init(&lcd1_expander, &lcd1_handle, ZH_LCD_16X2);
    // Initialize second LCD at address 0x26
    config.i2c_address = 0x26;
    zh_pcf8574_init(&config, &lcd2_expander);
    zh_160x_init(&lcd2_expander, &lcd2_handle, ZH_LCD_16X2);
    // Display on first LCD
    zh_160x_set_cursor(&lcd1_handle, 0, 0);
    zh_160x_print_char(&lcd1_handle, "LCD 1");
    // Display on second LCD
    zh_160x_set_cursor(&lcd2_handle, 0, 0);
    zh_160x_print_char(&lcd2_handle, "LCD 2");
    // Cleanup
    zh_160x_deinit(&lcd1_handle);
    zh_160x_deinit(&lcd2_handle);
}
```

---

## Technical Specifications

| Parameter | Value |
| ----------- | ------- |
| **LCD Types** | 1602A (16x2), 1604A (16x4) |
| **Interface** | I2C via PCF8574(A) |
| **I2C Address Range** | 0x20-0x27 (PCF8574), 0x38-0x3F (PCF8574A) |
| **Max LCDs per Bus** | 16 |
| **GPIO Usage** | 2 (SCL, SDA) |
| **ESP-IDF Version** | >= 5.0 |
| **Platform** | ESP32 series |
| **Language** | C (C99) |

---

## Error Codes

| Error Code | Description |
| ------------ | ------------- |
| `ESP_OK` | Operation completed successfully |
| `ESP_ERR_INVALID_ARG` | Invalid argument (NULL pointer or invalid value) |
| `ESP_ERR_INVALID_STATE` | Handle not initialized |
| `ESP_FAIL` | General failure (I2C communication error) |

---

## Connection

### 1602A/1604A to PCF8574 Connection

| LCD Pin | PCF8574 Pin | Description |
|---------|-------------|-------------|
| RS      | P0          | Register select |
| E       | P2          | Enable |
| D4      | P4          | Data bit 4 |
| D5      | P5          | Data bit 5 |
| D6      | P6          | Data bit 6 |
| D7      | P7          | Data bit 7 |

### I2C Connection

| LCD Module | PCF8574 | ESP32 |
|------------|---------|-------|
| VCC        | VCC     | 3.3V  |
| GND        | GND     | GND   |
| SDA        | SDA     | GPIO21 |
| SCL        | SCL     | GPIO22 |
| RW         | GND     | GND   |

**Note:** RW pin must be connected to GND for write-only operation.

---

## Contributing

Contributions are welcome! To contribute:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

Please ensure your code follows the existing style and includes appropriate documentation.

---

## License

This project is licensed under the Apache License, Version 2.0 - see the [LICENSE](LICENSE) file for details.

### Apache License, Version 2.0

Copyright (c) 2026 Alexey Zholtikov

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at:

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

---

## Additional Notes

- **I2C Pull-up Resistors**: Use 4.7kΩ resistors on SDA and SCL lines for reliable communication
- **Power Supply**: Ensure stable 5V power for LCD module (PCF8574 is 5V tolerant)
- **Contrast Adjustment**: Use the built-in potentiometer to adjust LCD contrast for optimal visibility
- **Backlight**: LCD backlight is typically connected to VCC through a current-limiting resistor
- **Performance**: Operations are blocking and use FreeRTOS delays for proper timing
- **I2C_ISR_IRAM_SAFE**: For proper operation, enable `I2C_ISR_IRAM_SAFE` and `I2C_MASTER_ISR_HANDLER_IN_IRAM` in menuconfig
- **FreeRTOS**: For correct operation, configure `CONFIG_FREERTOS_HZ=1000` in menuconfig
- **Best Practices**:
  - Always initialize PCF8574 before initializing LCD
  - Check I2C address using a scanner if LCD does not respond
  - Consider I2C bus speed limitations when using multiple devices

---

*Generated for zh_160x_i2c v5.0.0*
