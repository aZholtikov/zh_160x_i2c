# ESP32 ESP-IDF component for liquid crystal display module 1602(4)A via I2C connection (PCF8574(A))

## Tested on

1. [ESP32 ESP-IDF v5.5.2](https://docs.espressif.com/projects/esp-idf/en/v5.5.2/esp32/index.html)

## SAST Tools

[PVS-Studio](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.

## Features

1. Support of 16 LCD 160X on one bus.

## Attention

For correct operation, please enable the following settings in the menuconfig:

```text
CONFIG_FREERTOS_HZ=1000
CONFIG_GPIO_CTRL_FUNC_IN_IRAM
CONFIG_I2C_ISR_IRAM_SAFE
CONFIG_I2C_MASTER_ISR_HANDLER_IN_IRAM
CONFIG_I2C_ENABLE_SLAVE_DRIVER_VERSION_2
```

## Connection

| 1602(4)A | PCF8574 |
| -------- | ------- |
|    RS    |   P0    |
|    E     |   P2    |
|    D4    |   P4    |
|    D5    |   P5    |
|    D6    |   P6    |
|    D7    |   P7    |

## Dependencies

1. [zh_vector](http://git.zh.com.ru/esp_components/zh_vector)
2. [zh_pcf8574](http://git.zh.com.ru/esp_components/zh_pcf8574)

## Using

In an existing project, run the following command to install the components:

```text
cd ../your_project/components
git clone http://git.zh.com.ru/esp_components/zh_160x_i2c
git clone http://git.zh.com.ru/esp_components/zh_pcf8574
git clone http://git.zh.com.ru/esp_components/zh_vector
```

In the application, add the component:

```c
#include "zh_160x_i2c.h"
```

## Examples

One LCD on bus:

```c
#include "zh_160x_i2c.h"

#define I2C_PORT (I2C_NUM_MAX - 1)

i2c_master_bus_handle_t i2c_bus_handle = NULL;

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
    i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handle);
    zh_pcf8574_init_config_t config = ZH_PCF8574_INIT_CONFIG_DEFAULT();
    config.i2c_handle = i2c_bus_handle;
    config.i2c_address = 0x27;
    zh_pcf8574_init(&config, &lcd_160x_handle);
    zh_160x_init(&lcd_160x_handle, ZH_LCD_16X2);
    for (;;)
    {
        zh_160x_set_cursor(&lcd_160x_handle, 0, 0);
        zh_160x_print_char(&lcd_160x_handle, "LCD 160X");
        zh_160x_set_cursor(&lcd_160x_handle, 1, 0);
        zh_160x_print_char(&lcd_160x_handle, "Hello World!");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
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
