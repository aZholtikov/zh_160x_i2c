# zh_160x_i2c - ESP32 ESP-IDF Component for LCD 1602A/1604A via I2C (PCF8574)

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

1. **Simple API**: Easy-to-use functions for common LCD operations
2. **I2C Interface**: Uses only 2 GPIO pins (SCL and SDA) via PCF8574 expander
3. **Support for 1602A and 1604A**: Supports both 16x2 and 16x4 LCD modules
4. **Multiple LCD Support**: Up to 8 LCD modules on one I2C bus (PCF8574) or 4 (PCF8574A)
5. **Text Display**: Print strings, integers, and floating-point numbers
6. **Cursor Control**: Enable/disable cursor with optional blink
7. **Progress Bar**: Display progress bars on LCD
8. **Row Operations**: Clear specific rows
9. **ESP-IDF Optimized**: Uses ESP-IDF's I2C master driver and FreeRTOS

---

## Installation

1. Navigate to your project's components directory:

```bash
cd ../your_project/components
```

2. Clone the repository:

```bash
git clone https://github.com/aZholtikov/zh_160x_i2c
```

3. In your application, include the header:

```c
#include "zh_160x_i2c.h"
```

4. The component will be automatically built with your project.

### Required menuconfig Settings

For correct operation, enable the following settings in menuconfig:

```text
CONFIG_FREERTOS_HZ=1000
CONFIG_I2C_ISR_IRAM_SAFE
CONFIG_I2C_MASTER_ISR_HANDLER_IN_IRAM
```

---

## API Reference

### Configuration Constants

```c
#define ZH_LCD_16X2 1  // LCD size 16x2
#define ZH_LCD_16X4 0  // LCD size 16x4
```

---

### zh_160x_init()

Initializes the LCD 160X module.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle (must be already initialized)
- `size` - LCD size (ZH_LCD_16X2 or ZH_LCD_16X4)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle or handle not initialized)
- `ESP_FAIL` - Initialization failed

**Example:**

```c
zh_pcf8574_handle_t lcd_handle = {0};
// Initialize PCF8574 first
zh_pcf8574_init(&config, &lcd_handle);
// Then initialize LCD
zh_160x_init(&lcd_handle, ZH_LCD_16X2);
```

---

### zh_160x_lcd_clear()

Clears the entire LCD screen.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - Operation failed

---

### zh_160x_set_cursor()

Sets the cursor to a specific position on the LCD.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle
- `row` - Row number (0-1 for 16x2, 0-3 for 16x4)
- `col` - Column number (0-15)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle, invalid row/col)
- `ESP_FAIL` - Operation failed

---

### zh_160x_print_char()

Prints a string to the LCD.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle
- `str` - Pointer to the null-terminated string to display

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle or str)
- `ESP_FAIL` - Operation failed

---

### zh_160x_print_int()

Prints an integer to the LCD.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle
- `num` - Integer value to display

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - Operation failed

---

### zh_160x_print_float()

Prints a floating-point number to the LCD.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle
- `num` - Floating-point value to display
- `precision` - Number of decimal places (0-9)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - Operation failed

---

### zh_160x_print_progress_bar()

Displays a progress bar on a specific row of the LCD.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle
- `row` - Row number (0-1 for 16x2, 0-3 for 16x4)
- `progress` - Progress percentage (0-100)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle, invalid row or progress)
- `ESP_FAIL` - Operation failed

---

### zh_160x_clear_row()

Clears a specific row on the LCD.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle
- `row` - Row number (0-1 for 16x2, 0-3 for 16x4)

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle, invalid row)
- `ESP_FAIL` - Operation failed

---

### zh_160x_on_cursor()

Enables the cursor with optional blink mode.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle
- `blink` - If true, cursor blinks; if false, cursor is steady

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - Operation failed

---

### zh_160x_off_cursor()

Disables the cursor.

**Parameters:**

- `handle` - Pointer to unique PCF8574 handle

**Returns:**

- `ESP_OK` - Success
- `ESP_ERR_INVALID_ARG` - Invalid argument (NULL handle)
- `ESP_FAIL` - Operation failed

---

## Usage Examples

### Basic Example: Single LCD Display

```c
#include "zh_160x_i2c.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_pcf8574_handle_t lcd_160x_handle = {0};

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
    zh_pcf8574_init(&config, &lcd_160x_handle);
    zh_160x_init(&lcd_160x_handle, ZH_LCD_16X2);
    // Display text
    zh_160x_set_cursor(&lcd_160x_handle, 0, 0);
    zh_160x_print_char(&lcd_160x_handle, "LCD 160X");
    zh_160x_set_cursor(&lcd_160x_handle, 1, 0);
    zh_160x_print_char(&lcd_160x_handle, "Hello World!");
}
```

### Example: Progress Bar

```c
#include "zh_160x_i2c.h"

void app_main(void)
{
    zh_pcf8574_handle_t lcd_handle = {0};
    // ... initialization code ...
    for (uint8_t i = 0; i <= 100; ++i)
    {
        zh_160x_set_cursor(&lcd_handle, 0, 10);
        zh_160x_print_int(&lcd_handle, i);
        zh_160x_print_char(&lcd_handle, "%");
        zh_160x_print_progress_bar(&lcd_handle, 1, i);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
```

### Example: Displaying Various Data Types

```c
#include "zh_160x_i2c.h"

void app_main(void)
{
    zh_pcf8574_handle_t lcd_handle = {0};
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
}
```

### Example: Multiple LCDs on One Bus

```c
#include "zh_160x_i2c.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_pcf8574_handle_t lcd1_handle = {0};
zh_pcf8574_handle_t lcd2_handle = {0};

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
    zh_pcf8574_init(&config, &lcd1_handle);
    zh_160x_init(&lcd1_handle, ZH_LCD_16X2);
    // Initialize second LCD at address 0x26
    config.i2c_address = 0x26;
    zh_pcf8574_init(&config, &lcd2_handle);
    zh_160x_init(&lcd2_handle, ZH_LCD_16X2);
    // Display on first LCD
    zh_160x_set_cursor(&lcd1_handle, 0, 0);
    zh_160x_print_char(&lcd1_handle, "LCD 1");
    // Display on second LCD
    zh_160x_set_cursor(&lcd2_handle, 0, 0);
    zh_160x_print_char(&lcd2_handle, "LCD 2");
}
```

---

## Technical Specifications

| Parameter | Value |
|-----------|-------|
| **LCD Types** | 1602A (16x2), 1604A (16x4) |
| **Interface** | I2C via PCF8574(A) |
| **I2C Address Range** | 0x20-0x27 (PCF8574), 0x38-0x3F (PCF8574A) |
| **Max LCDs per Bus** | 16 |
| **GPIO Usage** | 2 (SCL, SDA) |
| **ESP-IDF Version** | >= 5.0|
| **Platform** | ESP32 family |
| **Language** | C (C99) |

---

## Error Codes

| Error Code | Description |
|------------|-------------|
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
