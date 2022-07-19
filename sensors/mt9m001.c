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

int mt9m001_init(sensor_t *sensor)
{
    sensor->reset = reset;
    sensor->init_status = init_status;
    sensor->set_pixformat = set_pixformat;
    sensor->set_framesize = set_framesize;
    sensor->set_contrast  = set_contrast;
    sensor->set_brightness= set_brightness;
    sensor->set_saturation= set_saturation;

    sensor->set_quality = set_quality;
    sensor->set_colorbar = set_colorbar;

    sensor->set_gainceiling = set_gainceiling_sensor;
    sensor->set_gain_ctrl = set_agc_sensor;
    sensor->set_exposure_ctrl = set_aec_sensor;
    sensor->set_hmirror = set_hmirror_sensor;
    sensor->set_vflip = set_vflip_sensor;

    sensor->set_whitebal = set_awb_dsp;
    sensor->set_aec2 = set_aec2;
    sensor->set_aec_value = set_aec_value;
    sensor->set_special_effect = set_special_effect;
    sensor->set_wb_mode = set_wb_mode;
    sensor->set_ae_level = set_ae_level;

    sensor->set_dcw = set_dcw_dsp;
    sensor->set_bpc = set_bpc_dsp;
    sensor->set_wpc = set_wpc_dsp;
    sensor->set_awb_gain = set_awb_gain_dsp;
    sensor->set_agc_gain = set_agc_gain;

    sensor->set_raw_gma = set_raw_gma_dsp;
    sensor->set_lenc = set_lenc_dsp;

    //not supported
    sensor->set_sharpness = set_sharpness;
    sensor->set_denoise = set_denoise;

    sensor->get_reg = get_reg;
    sensor->set_reg = set_reg;
    sensor->set_res_raw = set_res_raw;
    sensor->set_pll = _set_pll;
    sensor->set_xclk = set_xclk;
    ESP_LOGD(TAG, "MT9M001 Attached");
    return 0;
}