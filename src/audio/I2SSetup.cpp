#include <Arduino.h>
#include <driver/i2s.h>
#include "I2SSetup.h"

#define I2S_PORT   I2S_NUM_0

#if 0
#define I2S_BCK_IO 15
#define I2S_WS_IO  16
#define I2S_DO_IO  17
#else
#define I2S_BCK_IO 3
#define I2S_WS_IO  2
#define I2S_DO_IO  4
#endif

void setupI2S() {

	Serial.println("Installing I2S driver...");

	i2s_config_t cfg = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
		.sample_rate = 8000,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = I2S_COMM_FORMAT_STAND_I2S,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
		.dma_buf_count = 8,
		.dma_buf_len = 256,
		.use_apll = false,
		.tx_desc_auto_clear = true,
		.fixed_mclk = 0
	};

	i2s_pin_config_t pins = {
		.bck_io_num = I2S_BCK_IO,
		.ws_io_num = I2S_WS_IO,
		.data_out_num = I2S_DO_IO,
		.data_in_num = I2S_PIN_NO_CHANGE
	};

	esp_err_t err = i2s_driver_install(I2S_PORT, &cfg, 0, nullptr);
	if (err != ESP_OK) {
		Serial.println("I2S install failed");
		while (true);
	}

	i2s_set_pin(I2S_PORT, &pins);
	i2s_zero_dma_buffer(I2S_PORT);

	Serial.println("I2S ready");
}
