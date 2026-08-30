# ESP32 ESP-IDF component for LCD 1602A/1604A via I2C (PCF8574)

## SAST Tools

[PVS-Studio](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) — static analyzer for C, C++, C#, and Java code.

## Features

1. Support for up to 16 LCD 160X displays on one I2C bus.
2. 4-bit interface mode for efficient data transfer via PCF8574 expander.
3. Print strings, integers, and floating-point numbers.
4. Progress bar display with custom characters.
5. Cursor visibility and blink control.
6. Row-specific clear operations.

## Note

Enable the following settings in menuconfig:

```text
CONFIG_FREERTOS_HZ=1000
CONFIG_I2C_ISR_IRAM_SAFE
CONFIG_I2C_MASTER_ISR_HANDLER_IN_IRAM
```

## Using

In an existing project, run the following command to install the component:

```bash
cd ../your_project/components
git clone https://github.com/aZholtikov/zh_160x_i2c
```

In the application, add the component:

```c
#include "zh_160x_i2c.h"
```

## Example

```c
#include "zh_160x_i2c.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

zh_pcf8574_handle_t *pcf8574_handle = NULL;
zh_160x_i2c_handle_t *lcd_160x_handle = NULL;

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
    zh_pcf8574_init(&config, &pcf8574_handle);
    zh_160x_init(&pcf8574_handle, &lcd_160x_handle, ZH_LCD_16X2);
    for (;;)
    {
        zh_160x_on_cursor(&lcd_160x_handle, true);
        zh_160x_set_cursor(&lcd_160x_handle, 0, 0);
        zh_160x_print_char(&lcd_160x_handle, "LCD 160X");
        zh_160x_set_cursor(&lcd_160x_handle, 1, 0);
        zh_160x_print_char(&lcd_160x_handle, "Hello World!");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        zh_160x_off_cursor(&lcd_160x_handle);
        zh_160x_set_cursor(&lcd_160x_handle, 0, 0);
        zh_160x_print_char(&lcd_160x_handle, "Progress: ");
        for (uint8_t i = 0; i <= 100; ++i)
        {
            zh_160x_set_cursor(&lcd_160x_handle, 0, 10);
            zh_160x_print_int(&lcd_160x_handle, i);
            zh_160x_print_char(&lcd_160x_handle, "%");
            zh_160x_print_progress_bar(&lcd_160x_handle, 1, i);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        zh_160x_lcd_clear(&lcd_160x_handle);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
```
