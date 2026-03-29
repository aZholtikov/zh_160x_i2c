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

esp_err_t zh_160x_init(zh_pcf8574_handle_t *handle, bool size) // -V2008
{
    ZH_LOGI("160X initialization started.");
    ZH_ERROR_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X initialization failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X initialization failed. PCF8574 not initialized.");
    handle->system = heap_caps_calloc(1, sizeof(uint8_t), MALLOC_CAP_8BIT);
    ZH_ERROR_CHECK(handle->system != NULL, ESP_ERR_INVALID_ARG, NULL, "160X initialization failed. Memory allocation fail.");
    *(uint8_t *)handle->system = size;
    ZH_ERROR_CHECK(_zh_160x_lcd_init(handle) == ESP_OK, ESP_FAIL, NULL, "160X initialization failed. PCF8574 error.");
    ZH_LOGI("160X initialization completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_lcd_clear(zh_pcf8574_handle_t *handle)
{
    ZH_LOGI("160X display cleaning started.");
    ZH_ERROR_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X display cleaning failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X display cleaning failed. PCF8574 not initialized.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, 0x01) == ESP_OK, ESP_FAIL, NULL, "160X display cleaning failed. PCF8574 error.");
    ZH_LOGI("160X display cleaning completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_set_cursor(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t col) // -V2008
{
    ZH_LOGI("160X set cursor started.");
    ZH_ERROR_CHECK(handle != NULL && row < ((*(uint8_t *)handle->system == ZH_LCD_16X2) ? 2 : 4) && col < 16, ESP_ERR_INVALID_ARG, NULL, "160X set cursor failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X set cursor failed. PCF8574 not initialized.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, 0x80 | ((row == 0) ? col : (row == 1) ? (0x40 + col)
                                                                        : (row == 2)   ? (0x10 + col)
                                                                                       : (0x50 + col))) == ESP_OK,
                   ESP_FAIL, NULL, "160X set cursor failed. PCF8574 error.");
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
        ZH_ERROR_CHECK(_zh_160x_send_data(handle, (uint8_t)*str++) == ESP_OK, ESP_FAIL, NULL, "160X print char failed. PCF8574 error.");
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
    ZH_ERROR_CHECK(zh_160x_print_char(handle, buffer) == ESP_OK, ESP_FAIL, NULL, "160X print int failed. PCF8574 error.");
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
    ZH_ERROR_CHECK(zh_160x_print_char(handle, buffer) == ESP_OK, ESP_FAIL, NULL, "160X print float failed. PCF8574 error.");
    ZH_LOGI("160X print float completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_print_progress_bar(zh_pcf8574_handle_t *handle, uint8_t row, uint8_t progress) // -V2008
{
    ZH_LOGI("160X print progress bar started.");
    ZH_ERROR_CHECK(handle != NULL && row < ((*(uint8_t *)handle->system == ZH_LCD_16X2) ? 2 : 4) && progress <= 100, ESP_ERR_INVALID_ARG, NULL, "160X print progress bar failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X print progress bar failed. PCF8574 not initialized.");
    uint8_t blocks = (progress * 16) / 100;
    ZH_ERROR_CHECK(zh_160x_set_cursor(handle, row, 0) == ESP_OK, ESP_FAIL, NULL, "160X print progress bar failed. PCF8574 error.");
    for (uint8_t i = 0; i < 16; ++i)
    {
        if (i < blocks)
        {
            ZH_ERROR_CHECK(zh_160x_print_char(handle, "\xFF") == ESP_OK, ESP_FAIL, NULL, "160X print progress bar failed. PCF8574 error.");
        }
        else
        {
            ZH_ERROR_CHECK(zh_160x_print_char(handle, " ") == ESP_OK, ESP_FAIL, NULL, "160X print progress bar failed. PCF8574 error.");
        }
    }
    ZH_LOGI("160X print progress bar completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_clear_row(zh_pcf8574_handle_t *handle, uint8_t row) // -V2008
{
    ZH_LOGI("160X clear row started.");
    ZH_ERROR_CHECK(handle != NULL && row < ((*(uint8_t *)handle->system == ZH_LCD_16X2) ? 2 : 4), ESP_ERR_INVALID_ARG, NULL, "160X clear row failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X clear row failed. PCF8574 not initialized.");
    ZH_ERROR_CHECK(zh_160x_set_cursor(handle, row, 0) == ESP_OK, ESP_FAIL, NULL, "160X clear row failed. PCF8574 error.");
    for (uint8_t i = 0; i < 16; ++i)
    {
        ZH_ERROR_CHECK(zh_160x_print_char(handle, " ") == ESP_OK, ESP_FAIL, NULL, "160X clear row failed. PCF8574 error.");
    }
    ZH_ERROR_CHECK(zh_160x_set_cursor(handle, row, 0) == ESP_OK, ESP_FAIL, NULL, "160X clear row failed. PCF8574 error.");
    ZH_LOGI("160X clear row completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_on_cursor(zh_pcf8574_handle_t *handle, bool blink)
{
    ZH_LOGI("160X enable cursor started.");
    ZH_ERROR_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X enable cursor failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X enable cursor failed. PCF8574 not initialized.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, (blink == true) ? 0x0f : 0x0e) == ESP_OK, ESP_FAIL, NULL, "160X enable cursor failed. PCF8574 error.");
    ZH_LOGI("160X enable cursor completed successfully.");
    return ESP_OK;
}

esp_err_t zh_160x_off_cursor(zh_pcf8574_handle_t *handle)
{
    ZH_LOGI("160X disable cursor started.");
    ZH_ERROR_CHECK(handle != NULL, ESP_ERR_INVALID_ARG, NULL, "160X disable cursor failed. Invalid argument.");
    ZH_ERROR_CHECK(handle->is_initialized == true, ESP_ERR_INVALID_STATE, NULL, "160X disable cursor failed. PCF8574 not initialized.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, 0x0c) == ESP_OK, ESP_FAIL, NULL, "160X disable cursor failed. PCF8574 error.");
    ZH_LOGI("160X disable cursor completed successfully.");
    return ESP_OK;
}

static esp_err_t _zh_160x_lcd_init(zh_pcf8574_handle_t *handle) // -V2008
{
    vTaskDelay(20 / portTICK_PERIOD_MS);
    ZH_ERROR_CHECK(zh_pcf8574_write(handle, 0x30) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_pulse(handle) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(zh_pcf8574_write(handle, 0x30) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_pulse(handle) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(zh_pcf8574_write(handle, 0x30) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_pulse(handle) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(zh_pcf8574_write(handle, 0x20) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_pulse(handle) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, 0x28) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, 0x28) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, 0x28) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, 0x0C) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, 0x01) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_send_command(handle, 0x06) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    return ESP_OK;
}

static esp_err_t _zh_160x_send_command(zh_pcf8574_handle_t *handle, uint8_t command)
{
    ZH_ERROR_CHECK(zh_pcf8574_write(handle, (command & 0xF0) | 0x08) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_pulse(handle) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(zh_pcf8574_write(handle, (command << 4) | 0x08) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_pulse(handle) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    return ESP_OK;
}

static esp_err_t _zh_160x_send_data(zh_pcf8574_handle_t *handle, uint8_t data)
{
    ZH_ERROR_CHECK(zh_pcf8574_write(handle, (data & 0xF0) | 0x09) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_pulse(handle) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(zh_pcf8574_write(handle, (data << 4) | 0x09) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    ZH_ERROR_CHECK(_zh_160x_pulse(handle) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    return ESP_OK;
}

static esp_err_t _zh_160x_pulse(zh_pcf8574_handle_t *handle)
{
    ZH_ERROR_CHECK(zh_pcf8574_write_gpio(handle, ZH_PCF8574_GPIO_NUM_P2, ZH_PCF8574_GPIO_HIGH) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    vTaskDelay(1 / portTICK_PERIOD_MS);
    ZH_ERROR_CHECK(zh_pcf8574_write_gpio(handle, ZH_PCF8574_GPIO_NUM_P2, ZH_PCF8574_GPIO_LOW) == ESP_OK, ESP_FAIL, NULL, "PCF8574 error.");
    vTaskDelay(1 / portTICK_PERIOD_MS);
    return ESP_OK;
}
