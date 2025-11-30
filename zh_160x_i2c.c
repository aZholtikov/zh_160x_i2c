#include "zh_160x_i2c.h"

static const char *TAG = "zh_160x_i2c";

#define ZH_160X_I2C_LOGI(msg, ...) ESP_LOGI(TAG, msg, ##__VA_ARGS__)
#define ZH_160X_I2C_LOGW(msg, ...) ESP_LOGW(TAG, msg, ##__VA_ARGS__)
#define ZH_160X_I2C_LOGE(msg, ...) ESP_LOGE(TAG, msg, ##__VA_ARGS__)
#define ZH_160X_I2C_LOGE_ERR(msg, err, ...) ESP_LOGE(TAG, "[%s:%d:%s] " msg, __FILE__, __LINE__, esp_err_to_name(err), ##__VA_ARGS__)

#define ZH_160X_I2C_CHECK(cond, err, msg, ...) \
    if (!(cond))                               \
    {                                          \
        ZH_160X_I2C_LOGE_ERR(msg, err);        \
        return err;                            \
    }

#ifdef CONFIG_IDF_TARGET_ESP8266
#define esp_delay_us(x) os_delay_us(x)
#else
#define esp_delay_us(x) esp_rom_delay_us(x)
#endif

#define LCD_160X_PULSE                       \
    zh_pcf8574_write_gpio(handle, 2, true);  \
    esp_delay_us(500);                       \
    zh_pcf8574_write_gpio(handle, 2, false); \
    esp_delay_us(500);

static void _zh_160x_lcd_init(zh_pcf8574_handle_t *handle);
static void _zh_160x_send_command(zh_pcf8574_handle_t *handle, uint8_t command);
static void _zh_160x_send_data(zh_pcf8574_handle_t *handle, uint8_t data);

esp_err_t zh_160x_init(zh_pcf8574_handle_t *handle, bool size)
{
    ZH_160X_I2C_LOGI("160X initialization started.");
    ZH_160X_I2C_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, "160X initialization failed. Invalid argument.");
    ZH_160X_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "160X initialization failed. PCF8574 not initialized.");
    handle->system = heap_caps_calloc(1, sizeof(uint8_t), MALLOC_CAP_8BIT);
    ZH_160X_I2C_CHECK(handle->system != NULL, ESP_ERR_INVALID_ARG, "160X initialization failed. Memory allocation fail.");
    *(uint8_t *)handle->system = size;
    _zh_160x_lcd_init(handle);
    ZH_160X_I2C_LOGI("160X initialization completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_lcd_clear(zh_pcf8574_handle_t *handle)
{
    ZH_160X_I2C_LOGI("160X display cleaning started.");
    ZH_160X_I2C_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, "160X display cleaning failed. Invalid argument.");
    ZH_160X_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "160X display cleaning failed. PCF8574 not initialized.");
    _zh_160x_send_command(handle, 0x01);
    ZH_160X_I2C_LOGI("160X display cleaning completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_set_cursor(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t col)
{
    ZH_160X_I2C_LOGI("160X set cursor started.");
    ZH_160X_I2C_CHECK(row < ((*(uint8_t *)handle->system == ZH_LCD_16X2) ? 2 : 4) && col < 16 && handle != NULL, ESP_ERR_INVALID_ARG, "160X set cursor failed. Invalid argument.");
    ZH_160X_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "160X set cursor failed. PCF8574 not initialized.");
    _zh_160x_send_command(handle, 0x80 | ((row == 0) ? col : (row == 1) ? (0x40 + col)
                                                         : (row == 2)   ? (0x10 + col)
                                                                        : (0x50 + col)));
    ZH_160X_I2C_LOGI("160X set cursor completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_print_char(zh_pcf8574_handle_t *handle, const char *str)
{
    ZH_160X_I2C_LOGI("160X print char started.");
    ZH_160X_I2C_CHECK(str != NULL && handle != NULL, ESP_ERR_INVALID_ARG, "160X print char failed. Invalid argument.");
    ZH_160X_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "160X print char failed. PCF8574 not initialized.");
    while (*str != 0)
    {
        _zh_160x_send_data(handle, (uint8_t)*str++);
    }
    ZH_160X_I2C_LOGI("160X print char completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_print_int(zh_pcf8574_handle_t *handle, int num)
{
    ZH_160X_I2C_LOGI("160X print int started.");
    ZH_160X_I2C_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, "160X print int failed. Invalid argument.");
    ZH_160X_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "160X print int failed. PCF8574 not initialized.");
    char buffer[12];
    sprintf(buffer, "%d", num);
    zh_160x_print_char(handle, buffer);
    ZH_160X_I2C_LOGI("160X print int completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_print_float(zh_pcf8574_handle_t *handle, float num, uint8_t precision)
{
    ZH_160X_I2C_LOGI("160X print float started.");
    ZH_160X_I2C_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, "160X print float failed. Invalid argument.");
    ZH_160X_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "160X print float failed. PCF8574 not initialized.");
    char buffer[16];
    sprintf(buffer, "%.*f", precision, num);
    zh_160x_print_char(handle, buffer);
    ZH_160X_I2C_LOGI("160X print float completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_print_progress_bar(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t progress)
{
    ZH_160X_I2C_LOGI("160X print progress bar started.");
    ZH_160X_I2C_CHECK(row < ((*(uint8_t *)handle->system == ZH_LCD_16X2) ? 2 : 4) && progress <= 100 && handle != NULL, ESP_ERR_INVALID_ARG, "160X print progress bar failed. Invalid argument.");
    ZH_160X_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "160X print progress bar failed. PCF8574 not initialized.");
    uint8_t blocks = (progress * 16) / 100;
    zh_160x_set_cursor(handle, row, 0);
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (i < blocks)
        {
            zh_160x_print_char(handle, "\xFF");
        }
        else
        {
            zh_160x_print_char(handle, " ");
        }
    }
    ZH_160X_I2C_LOGI("160X print progress bar completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_clear_row(zh_pcf8574_handle_t *handle, uint8_t row)
{
    ZH_160X_I2C_LOGI("160X clear row started.");
    ZH_160X_I2C_CHECK(row < ((*(uint8_t *)handle->system == ZH_LCD_16X2) ? 2 : 4) && handle != NULL, ESP_ERR_INVALID_ARG, "160X clear row failed. Invalid argument.");
    ZH_160X_I2C_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, "160X clear row failed. PCF8574 not initialized.");
    zh_160x_set_cursor(handle, row, 0);
    for (uint8_t i = 0; i < 16; ++i)
    {
        zh_160x_print_char(handle, " ");
    }
    zh_160x_set_cursor(handle, row, 0);
    ZH_160X_I2C_LOGI("160X clear row completed successfully.");
    return ESP_OK;
}

static void _zh_160x_lcd_init(zh_pcf8574_handle_t *handle)
{
    vTaskDelay(20 / portTICK_PERIOD_MS);
    zh_pcf8574_write(handle, 0x30);
    LCD_160X_PULSE;
    zh_pcf8574_write(handle, 0x30);
    LCD_160X_PULSE;
    zh_pcf8574_write(handle, 0x30);
    LCD_160X_PULSE;
    zh_pcf8574_write(handle, 0x20);
    LCD_160X_PULSE;
    _zh_160x_send_command(handle, 0x28);
    _zh_160x_send_command(handle, 0x28);
    _zh_160x_send_command(handle, 0x28);
    _zh_160x_send_command(handle, 0x0C);
    _zh_160x_send_command(handle, 0x01);
    _zh_160x_send_command(handle, 0x06);
}

static void _zh_160x_send_command(zh_pcf8574_handle_t *handle, uint8_t command)
{
    zh_pcf8574_write(handle, (command & 0xF0) | 0x08);
    LCD_160X_PULSE;
    zh_pcf8574_write(handle, (command << 4) | 0x08);
    LCD_160X_PULSE;
}

static void _zh_160x_send_data(zh_pcf8574_handle_t *handle, uint8_t data)
{
    zh_pcf8574_write(handle, (data & 0xF0) | 0x09);
    LCD_160X_PULSE;
    zh_pcf8574_write(handle, (data << 4) | 0x09);
    LCD_160X_PULSE;
}
