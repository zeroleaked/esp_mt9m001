#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sccb.h"
#include "xclk.h"
#include "mt9m001.h"
#include "mt9m001_regs.h"
#include "mt9m001_settings.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#else
#include "esp_log.h"
static const char *TAG = "MT9M001";
#endif

static int reset(sensor_t *sensor)
{
    int ret = 0;
    SCCB_Write8_16(sensor->slv_addr, REG_RESET, 0x0001);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    SCCB_Write8_16(sensor->slv_addr, REG_RESET, 0x0000);
    return ret;
}

int mt9m001_detect(int slv_addr, sensor_id_t *id)
{
    if (MT9M001_SCCB_ADDR == slv_addr) {
        uint16_t PID = SCCB_Read8_16(slv_addr, REG_CHIP);
        if (MT9M001_PID == PID) {
            id->PID = PID;
            return PID;
        } else {
            ESP_LOGI(TAG, "Mismatch PID=0x%x", PID);
        }
    }
    return 0;
}

static int init_status(sensor_t *sensor)
{
    uint16_t octrl_status = SCCB_Read8_16(sensor->slv_addr, REG_OCTRL);
    SCCB_Write8_16(sensor->slv_addr, REG_OCTRL, (1<<6) | octrl_status);
    uint16_t test_data = SCCB_Read8_16(sensor->slv_addr, REG_TEST_DATA);
    ESP_LOGD(TAG, "Reg0x32: 0x%X", test_data);
    return 0;
}

static int set_dummy(sensor_t *sensor, int enable)
{
    ESP_LOGE(TAG, "Not support");
    return -1;
}

static int set_framesize(sensor_t *sensor, framesize_t framesize)
{
    int ret = SCCB_Write8_16(sensor->slv_addr, REG_RSIZE, resolution[framesize].height);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Frame height setting failed");
        return -1;
    }
    ret = SCCB_Write8_16(sensor->slv_addr, REG_CSIZE, resolution[framesize].width);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Frame width setting failed");
        return -1;
    }
    return 0;
}

static int set_pixformat(sensor_t *sensor, pixformat_t pixformat)
{
    int ret = 0;
    sensor->pixformat = pixformat;
    switch (pixformat) {
        case PIXFORMAT_GRAYSCALE:
            break;
        default:
            ESP_LOGE(TAG, "Not support");
            ret = -1;
            break;
    }

    return ret;
}

int mt9m001_init(sensor_t *sensor)
{
    sensor->reset = reset;
    sensor->init_status = init_status;
    sensor->set_pixformat = set_pixformat;
    sensor->set_framesize = set_framesize;
    sensor->set_contrast  = set_dummy;
    sensor->set_brightness= set_dummy;
    sensor->set_saturation= set_dummy;

    sensor->set_quality = set_dummy;
    sensor->set_colorbar = set_dummy;

    sensor->set_gain_ctrl = set_dummy;
    sensor->set_exposure_ctrl = set_dummy;
    sensor->set_hmirror = set_dummy;
    sensor->set_vflip = set_dummy;

    sensor->set_whitebal = set_dummy;
    sensor->set_aec2 = set_dummy;
    sensor->set_aec_value = set_dummy;
    sensor->set_special_effect = set_dummy;
    sensor->set_wb_mode = set_dummy;
    sensor->set_ae_level = set_dummy;

    sensor->set_dcw = set_dummy;
    sensor->set_bpc = set_dummy;
    sensor->set_wpc = set_dummy;
    sensor->set_awb_gain = set_dummy;
    sensor->set_agc_gain = set_dummy;

    sensor->set_raw_gma = set_dummy;
    sensor->set_lenc = set_dummy;

    //not supported
    sensor->set_sharpness = set_dummy;
    sensor->set_denoise = set_dummy;

    ESP_LOGD(TAG, "MT9M001 Attached");
    return 0;
}