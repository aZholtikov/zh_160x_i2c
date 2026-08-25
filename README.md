# ESP32 ESP-IDF component for LCD 1602A/1604A via I2C (PCF8574)

## Wiki

[EN](WIKI_EN.md) | [RU](WIKI_RU.md)

## Tested on

1. [ESP32 ESP-IDF v6.0.0](https://docs.espressif.com/projects/esp-idf/en/v6.0/esp32/index.html)

## SAST Tools

[PVS-Studio](https://pvs-studio.com/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) — static analyzer for C, C++, C#, and Java code.

## Features

1. Support for up to 16 LCD 160X displays on one I2C bus.
2. 4-bit interface mode for efficient data transfer via PCF8574 expander.
3. Print strings, integers, and floating-point numbers.
4. Progress bar display with custom characters.
5. Cursor visibility and blink control.
6. Row-specific clear operations.

## Using

In an existing project, run the following command to install the component:

```text
cd ../your_project/components
git clone https://github.com/aZholtikov/zh_160x_i2c
```

In the application, add the component:

```c
#include "zh_160x_i2c.h"
```

## Examples

See Wiki [EN](WIKI_EN.md#usage-examples) | [RU](WIKI_RU.md#примеры-использования)
