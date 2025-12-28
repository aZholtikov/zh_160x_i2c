#include "zh_160x_i2c.h"

static const char *TAG = "zh_160x_i2c";

#define ZH_LOGI(msg, ...) ESP_LOGI(TAG, msg, ##__VA_ARGS__)
#define ZH_LOGE(msg, err, ...) ESP_LOGE(TAG, "[%s:%d:%s] " msg, __FILE__, __LINE__, esp_err_to_name(err), ##__VA_ARGS__)

#define ZH_ERROR_CHECK(cond, err, cleanup, msg, ...) \
    if (!(cond))                                     \
    {                                                \
        ZH_LOGE(msg, err, ##__VA_ARGS__);            \
        cleanup;                                     \
        return err;                                  \
    }

static esp_err_t _zh_160x_lcd_init(zh_pcf8574_handle_t *handle);
static esp_err_t _zh_160x_send_command(zh_pcf8574_handle_t *handle, uint8_t command);
static esp_err_t _zh_160x_send_data(zh_pcf8574_handle_t *handle, uint8_t data);
static esp_err_t _zh_160x_pulse(zh_pcf8574_handle_t *handle);

esp_err_t zh_160x_init(zh_pcf8574_handle_t *handle, bool size)
{
    ZH_LOGI("160X initialization started.");
    ZH_ERROR_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X initialization failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X initialization failed. PCF8574 not initialized.");
    handle->system = heap_caps_calloc(1, sizeof(uint8_t), MALLOC_CAP_8BIT);
    ZH_ERROR_CHECK(handle->system != NULL, ESP_ERR_INVALID_ARG, NULL, "160X initialization failed. Memory allocation fail.");
    *(uint8_t *)handle->system = size;
    esp_err_t err = _zh_160x_lcd_init(handle);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X initialization failed. PCF8574 error.");
    ZH_LOGI("160X initialization completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_lcd_clear(zh_pcf8574_handle_t *handle)
{
    ZH_LOGI("160X display cleaning started.");
    ZH_ERROR_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X display cleaning failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X display cleaning failed. PCF8574 not initialized.");
    esp_err_t err = _zh_160x_send_command(handle, 0x01);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X display cleaning failed. PCF8574 error.");
    ZH_LOGI("160X display cleaning completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_set_cursor(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t col)
{
    ZH_LOGI("160X set cursor started.");
    ZH_ERROR_CHECK(row < ((*(uint8_t *)handle->system == ZH_LCD_16X2) ? 2 : 4) && col < 16 && handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X set cursor failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X set cursor failed. PCF8574 not initialized.");
    esp_err_t err = _zh_160x_send_command(handle, 0x80 | ((row == 0) ? col : (row == 1) ? (0x40 + col)
                                                                         : (row == 2)   ? (0x10 + col)
                                                                                        : (0x50 + col)));
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X set cursor failed. PCF8574 error.");
    ZH_LOGI("160X set cursor completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_print_char(zh_pcf8574_handle_t *handle, const char *str)
{
    ZH_LOGI("160X print char started.");
    ZH_ERROR_CHECK(str != NULL && handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X print char failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X print char failed. PCF8574 not initialized.");
    while (*str != 0)
    {
        esp_err_t err = _zh_160x_send_data(handle, (uint8_t)*str++);
        ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X print char failed. PCF8574 error.");
    }
    ZH_LOGI("160X print char completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_print_int(zh_pcf8574_handle_t *handle, int num)
{
    ZH_LOGI("160X print int started.");
    ZH_ERROR_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X print int failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X print int failed. PCF8574 not initialized.");
    char buffer[12];
    sprintf(buffer, "%d", num);
    esp_err_t err = zh_160x_print_char(handle, buffer);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X print int failed. PCF8574 error.");
    ZH_LOGI("160X print int completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_print_float(zh_pcf8574_handle_t *handle, float num, uint8_t precision)
{
    ZH_LOGI("160X print float started.");
    ZH_ERROR_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X print float failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X print float failed. PCF8574 not initialized.");
    char buffer[16];
    sprintf(buffer, "%.*f", precision, num);
    esp_err_t err = zh_160x_print_char(handle, buffer);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X print float failed. PCF8574 error.");
    ZH_LOGI("160X print float completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_print_progress_bar(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t progress)
{
    ZH_LOGI("160X print progress bar started.");
    ZH_ERROR_CHECK(row < ((*(uint8_t *)handle->system == ZH_LCD_16X2) ? 2 : 4) && progress <= 100 && handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X print progress bar failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X print progress bar failed. PCF8574 not initialized.");
    uint8_t blocks = (progress * 16) / 100;
    esp_err_t err = zh_160x_set_cursor(handle, row, 0);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X print progress bar failed. PCF8574 error.");
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (i < blocks)
        {
            err = zh_160x_print_char(handle, "\xFF");
            ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X print progress bar failed. PCF8574 error.");
        }
        else
        {
            err = zh_160x_print_char(handle, " ");
            ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X print progress bar failed. PCF8574 error.");
        }
    }
    ZH_LOGI("160X print progress bar completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_clear_row(zh_pcf8574_handle_t *handle, uint8_t row)
{
    ZH_LOGI("160X clear row started.");
    ZH_ERROR_CHECK(row < ((*(uint8_t *)handle->system == ZH_LCD_16X2) ? 2 : 4) && handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X clear row failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X clear row failed. PCF8574 not initialized.");
    esp_err_t err = zh_160x_set_cursor(handle, row, 0);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X clear row failed. PCF8574 error.");
    for (uint8_t i = 0; i < 16; ++i)
    {
        err = zh_160x_print_char(handle, " ");
        ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X clear row failed. PCF8574 error.");
    }
    err = zh_160x_set_cursor(handle, row, 0);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "160X clear row failed. PCF8574 error.");
    ZH_LOGI("160X clear row completed successfully.");
    return ESP_OK;
}

static esp_err_t _zh_160x_lcd_init(zh_pcf8574_handle_t *handle)
{
    vTaskDelay(20 / portTICK_PERIOD_MS);
    esp_err_t err = zh_pcf8574_write(handle, 0x30);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_pulse(handle);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = zh_pcf8574_write(handle, 0x30);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_pulse(handle);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = zh_pcf8574_write(handle, 0x30);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_pulse(handle);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = zh_pcf8574_write(handle, 0x20);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_pulse(handle);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_send_command(handle, 0x28);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_send_command(handle, 0x28);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_send_command(handle, 0x28);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_send_command(handle, 0x0C);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_send_command(handle, 0x01);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_send_command(handle, 0x06);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    return ESP_OK;
}

static esp_err_t _zh_160x_send_command(zh_pcf8574_handle_t *handle, uint8_t command)
{
    esp_err_t err = zh_pcf8574_write(handle, (command & 0xF0) | 0x08);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_pulse(handle);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = zh_pcf8574_write(handle, (command << 4) | 0x08);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_pulse(handle);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    return ESP_OK;
}

static esp_err_t _zh_160x_send_data(zh_pcf8574_handle_t *handle, uint8_t data)
{
    esp_err_t err = zh_pcf8574_write(handle, (data & 0xF0) | 0x09);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_pulse(handle);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = zh_pcf8574_write(handle, (data << 4) | 0x09);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    err = _zh_160x_pulse(handle);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    return ESP_OK;
}

static esp_err_t _zh_160x_pulse(zh_pcf8574_handle_t *handle)
{
    esp_err_t err = zh_pcf8574_write_gpio(handle, 2, true);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    vTaskDelay(1 / portTICK_PERIOD_MS);
    err = zh_pcf8574_write_gpio(handle, 2, false);
    ZH_ERROR_CHECK(err == ESP_OK, err, NULL, "PCF8574 error.");
    vTaskDelay(1 / portTICK_PERIOD_MS);
    return ESP_OK;
}
