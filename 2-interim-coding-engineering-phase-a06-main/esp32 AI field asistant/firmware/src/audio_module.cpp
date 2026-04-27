#include "audio_module.h"
#include <driver/i2s.h>

#define I2S_PORT I2S_NUM_0

void setup_audio_test() {
    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX), // Transmit mode
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };

    const i2s_pin_config_t pin_config = {
        .bck_io_num = 13,   // BCLK -> GPIO 13
        .ws_io_num = 12,    // LRC -> GPIO 12
        .data_out_num = 14, // DIN -> GPIO 14
        .data_in_num = -1   // Not used for output
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
}