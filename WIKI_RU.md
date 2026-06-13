# zh_160x_i2c - Компонент ESP32 ESP-IDF для LCD 1602A/1604A через I2C (PCF8574)

## Содержание

- [Обзор](#обзор)
- [Возможности](#возможности)
- [Установка](#установка)
- [Справочник API](#справочник-api)
- [Примеры использования](#примеры-использования)
- [Технические характеристики](#технические-характеристики)
- [Коды ошибок](#коды-ошибок)
- [Подключение](#подключение)
- [Вклад в проект](#вклад-в-проект)
- [Лицензия](#лицензия)

---

## Обзор

`zh_160x_i2c` — это легковесный компонент ESP-IDF для управления LCD-дисплеями 1602A и 1604A через интерфейс I2C с использованием расширителей ввода-вывода PCF8574(A). Этот компонент упрощает процесс отображения текста, чисел и индикаторов прогресса на символьных LCD-модулях, предоставляя простой и интуитивно понятный API.

Компонент разработан специально для микроконтроллеров ESP32 и использует драйвер I2C-мастера ESP-IDF и FreeRTOS для оптимальной производительности во встраиваемых системах.

---

## Возможности

1. **Простой API**: Удобные функции для распространенных операций с LCD
2. **Интерфейс I2C**: Использует только 2 GPIO-пина (SCL и SDA) через расширитель PCF8574
3. **Поддержка 1602A и 1604A**: Поддерживает LCD-модули 16x2 и 16x4
4. **Множественные LCD**: До 16 LCD-модулей на одной шине I2C
5. **Отображение текста**: Вывод строк, целых чисел и чисел с плавающей точкой
6. **Управление курсором**: Включение/выключение курсора с опциональным миганием
7. **Индикатор прогресса**: Отображение индикаторов прогресса на LCD
8. **Операции со строками**: Очистка конкретных строк
9. **Оптимизировано под ESP-IDF**: Использует драйвер I2C-мастера ESP-IDF и FreeRTOS

---

## Установка

1. Перейдите в каталог компонентов вашего проекта:

```bash
cd ../ваш_проект/components
```

2. Клонируйте репозиторий:

```bash
git clone https://github.com/aZholtikov/zh_160x_i2c
```

3. В вашем приложении подключите заголовочный файл:

```c
#include "zh_160x_i2c.h"
```

4. Компонент будет автоматически собран вместе с вашим проектом.

### Обязательные настройки в menuconfig

Для корректной работы компонента включите следующие настройки в menuconfig:

```text
CONFIG_FREERTOS_HZ=1000
CONFIG_I2C_ISR_IRAM_SAFE
CONFIG_I2C_MASTER_ISR_HANDLER_IN_IRAM
```

---

## Справочник API

### Константы конфигурации

```c
#define ZH_LCD_16X2 1  // Размер LCD 16x2
#define ZH_LCD_16X4 0  // Размер LCD 16x4
```

---

### zh_160x_init()

Инициализирует модуль LCD 160X.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574 (должен быть уже инициализирован)
- `size` - Размер LCD (ZH_LCD_16X2 или ZH_LCD_16X4)

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle или handle не инициализирован)
- `ESP_FAIL` - Ошибка инициализации

**Пример:**

```c
zh_pcf8574_handle_t lcd_handle = {0};
// Сначала инициализируйте PCF8574
zh_pcf8574_init(&config, &lcd_handle);
// Затем инициализируйте LCD
zh_160x_init(&lcd_handle, ZH_LCD_16X2);
```

---

### zh_160x_lcd_clear()

Очищает весь экран LCD.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle)
- `ESP_FAIL` - Операция не удалась

---

### zh_160x_set_cursor()

Устанавливает курсор в определенную позицию на LCD.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574
- `row` - Номер строки (0-1 для 16x2, 0-3 для 16x4)
- `col` - Номер столбца (0-15)

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle, неверный row/col)
- `ESP_FAIL` - Операция не удалась

---

### zh_160x_print_char()

Выводит строку на LCD.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574
- `str` - Указатель на null-terminated строку для отображения

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle или str)
- `ESP_FAIL` - Операция не удалась

---

### zh_160x_print_int()

Выводит целое число на LCD.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574
- `num` - Целочисленное значение для отображения

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle)
- `ESP_FAIL` - Операция не удалась

---

### zh_160x_print_float()

Выводит число с плавающей точкой на LCD.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574
- `num` - Значение с плавающей точкой для отображения
- `precision` - Количество знаков после запятой (0-9)

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle)
- `ESP_FAIL` - Операция не удалась

---

### zh_160x_print_progress_bar()

Отображает индикатор прогресса в определенной строке LCD.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574
- `row` - Номер строки (0-1 для 16x2, 0-3 для 16x4)
- `progress` - Процент прогресса (0-100)

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle, неверный row или progress)
- `ESP_FAIL` - Операция не удалась

---

### zh_160x_clear_row()

Очищает определенную строку на LCD.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574
- `row` - Номер строки (0-1 для 16x2, 0-3 для 16x4)

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle, неверный row)
- `ESP_FAIL` - Операция не удалась

---

### zh_160x_on_cursor()

Включает курсор с опциональным режимом мигания.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574
- `blink` - Если true, курсор мигает; если false, курсор статичный

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle)
- `ESP_FAIL` - Операция не удалась

---

### zh_160x_off_cursor()

Отключает курсор.

**Параметры:**

- `handle` - Указатель на уникальный дескриптор PCF8574

**Возвращает:**

- `ESP_OK` - Успех
- `ESP_ERR_INVALID_ARG` - Неверный аргумент (NULL handle)
- `ESP_FAIL` - Операция не удалась

---

## Примеры использования

### Базовый пример: Одиночный LCD-дисплей

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
    // Отображение текста
    zh_160x_set_cursor(&lcd_160x_handle, 0, 0);
    zh_160x_print_char(&lcd_160x_handle, "LCD 160X");
    zh_160x_set_cursor(&lcd_160x_handle, 1, 0);
    zh_160x_print_char(&lcd_160x_handle, "Привет Мир!");
}
```

### Пример: Индикатор прогресса

```c
#include "zh_160x_i2c.h"

void app_main(void)
{
    zh_pcf8574_handle_t lcd_handle = {0};
    // ... код инициализации ...
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

### Пример: Отображение различных типов данных

```c
#include "zh_160x_i2c.h"

void app_main(void)
{
    zh_pcf8574_handle_t lcd_handle = {0};
    // ... код инициализации ...
    // Отображение строки
    zh_160x_set_cursor(&lcd_handle, 0, 0);
    zh_160x_print_char(&lcd_handle, "Строка: ");
    zh_160x_print_char(&lcd_handle, "Привет");
    // Отображение целого числа
    zh_160x_set_cursor(&lcd_handle, 1, 0);
    zh_160x_print_int(&lcd_handle, 42);
    // Отображение числа с плавающей точкой
    zh_160x_set_cursor(&lcd_handle, 0, 0);
    zh_160x_print_float(&lcd_handle, 3.14159, 2);  // 3.14
    // Включение курсора
    zh_160x_on_cursor(&lcd_handle, true);  // Мигающий курсор
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    // Отключение курсора
    zh_160x_off_cursor(&lcd_handle);
}
```

### Пример: Несколько LCD на одной шине

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
    // Инициализация первого LCD по адресу 0x27
    zh_pcf8574_init_config_t config = ZH_PCF8574_INIT_CONFIG_DEFAULT();
    config.i2c_handle = i2c_bus_handle;
    config.i2c_address = 0x27;
    zh_pcf8574_init(&config, &lcd1_handle);
    zh_160x_init(&lcd1_handle, ZH_LCD_16X2);
    // Инициализация второго LCD по адресу 0x26
    config.i2c_address = 0x26;
    zh_pcf8574_init(&config, &lcd2_handle);
    zh_160x_init(&lcd2_handle, ZH_LCD_16X2);
    // Отображение на первом LCD
    zh_160x_set_cursor(&lcd1_handle, 0, 0);
    zh_160x_print_char(&lcd1_handle, "LCD 1");
    // Отображение на втором LCD
    zh_160x_set_cursor(&lcd2_handle, 0, 0);
    zh_160x_print_char(&lcd2_handle, "LCD 2");
}
```

---

## Технические характеристики

| Параметр | Значение |
|----------|----------|
| **Типы LCD** | 1602A (16x2), 1604A (16x4) |
| **Интерфейс** | I2C через PCF8574(A) |
| **Диапазон адресов I2C** | 0x20-0x27 (PCF8574), 0x38-0x3F (PCF8574A) |
| **Макс. LCD на шине** | 16 |
| **Использование GPIO** | 2 (SCL, SDA) |
| **Версия ESP-IDF** | >= 5.0 |
| **Платформа** | Семейство ESP32 |
| **Язык** | C (C99) |

---

## Коды ошибок

| Код ошибки | Описание |
|------------|----------|
| `ESP_OK` | Операция выполнена успешно |
| `ESP_ERR_INVALID_ARG` | Неверный аргумент (NULL указатель или неверное значение) |
| `ESP_ERR_INVALID_STATE` | Дескриптор не инициализирован |
| `ESP_FAIL` | Общий сбой (ошибка связи I2C) |

---

## Подключение

### Подключение 1602A/1604A к PCF8574

| Пин LCD | Пин PCF8574 | Описание |
|---------|-------------|----------|
| RS      | P0          | Выбор регистра |
| E       | P2          | Enable |
| D4      | P4          | Бит данных 4 |
| D5      | P5          | Бит данных 5 |
| D6      | P6          | Бит данных 6 |
| D7      | P7          | Бит данных 7 |

### Подключение I2C

| LCD-модуль | PCF8574 | ESP32 |
|------------|---------|-------|
| VCC        | VCC     | 3.3V  |
| GND        | GND     | GND   |
| SDA        | SDA     | GPIO21 |
| SCL        | SCL     | GPIO22 |
| RW         | GND     | GND   |

**Примечание:** Пин RW должен быть подключен к GND для операции только записи.

---

## Вклад в проект

Вклад приветствуется! Чтобы внести свой вклад:

1. Сделайте форк репозитория
2. Создайте ветку функции (`git checkout -b feature/AmazingFeature`)
3. Закоммитьте ваши изменения (`git commit -m 'Add some AmazingFeature'`)
4. Отправьте в ветку (`git push origin feature/AmazingFeature`)
5. Откройте Pull Request

Пожалуйста, убедитесь, что ваш код следует существующему стилю и включает соответствующую документацию.

---

## Лицензия

Этот проект лицензирован по лицензии Apache, версия 2.0 - см. файл [LICENSE](LICENSE) для подробной информации.

### Apache License, Version 2.0

Авторское право (c) 2026 Алексей Жолтиков

Лицензировано по лицензии Apache License, Version 2.0 (далее — "Лицензия");
вы не можете использовать этот файл, кроме случаев, предусмотренных Лицензией.
Копию Лицензии можно получить по адресу:

    http://www.apache.org/licenses/LICENSE-2.0

Если иное не требуется действующим законодательством или не согласовано в письменном виде,
программное обеспечение, распространяемое по Лицензии, распространяется на условиях "КАК ЕСТЬ",
БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ, явных или подразумеваемых, включая, но не ограничиваясь, гарантии
ТОВАРНОГО СОСТОЯНИЯ, ПРИГОДНОСТИ ДЛЯ КОНКРЕТНОЙ ЦЕЛИ И НЕНАРУШЕНИЯ ПРАВ.
Смотрите Лицензию для получения конкретных прав и ограничений.

---

## Дополнительные заметки

- **Подтягивающие резисторы I2C**: Используйте резисторы 4.7 кОм на линиях SDA и SCL для надежной связи
- **Питание**: Обеспечьте стабильное питание 5 В для LCD-модуля (PCF8574 совместим с 5 В)
- **Настройка контраста**: Используйте встроенный подстроечный резистор для настройки контраста LCD для оптимальной видимости
- **Подсветка**: Подсветка LCD обычно подключается к VCC через резистор ограничения тока
- **Производительность**: Операции блокирующие и используют задержки FreeRTOS для правильного тайминга
- **I2C_ISR_IRAM_SAFE**: Для правильной работы включите `I2C_ISR_IRAM_SAFE` и `I2C_MASTER_ISR_HANDLER_IN_IRAM` в menuconfig
- **FreeRTOS**: Для корректной работы настройте `CONFIG_FREERTOS_HZ=1000` в menuconfig
- **Лучшие практики**:
  - Всегда инициализируйте PCF8574 перед инициализацией LCD
  - Проверьте адрес I2C с помощью сканера, если LCD не отвечает
  - Учитывайте ограничения скорости шины I2C при использовании нескольких устройств
