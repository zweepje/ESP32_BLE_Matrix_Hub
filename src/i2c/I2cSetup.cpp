#include <Arduino.h>
#include <Wire.h>
#include "I2CSetup.h"

void setupI2C(int sda, int scl) {
	Wire.begin(sda, scl);
	Wire.setClock(400000);

	Serial.print("I2C started on SDA=");
	Serial.print(sda);
	Serial.print(" SCL=");
	Serial.println(scl);
}

#if 0
#include <Arduino.h>
#include <driver/i2s.h>

#define I2S_PORT     I2S_NUM_0

// Pins die bij jou werken
#define I2S_BCK_IO   15
#define I2S_WS_IO    16
#define I2S_DO_IO    17

#include <Arduino.h>
#include <Wire.h>
#include "I2CSetup.h"

void setupI2C(int sda, int scl) {
    Wire.begin(sda, scl);
    Wire.setClock(400000);

    Serial.print("I2C started on SDA=");
    Serial.print(sda);
    Serial.print(" SCL=");
    Serial.println(scl);
}
void setupI2S() {


	Serial.println("Installing I2S driver...");

	i2s_config_t i2s_config = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
		.sample_rate = 8000, // wordt later aangepast door WavPlayer
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = I2S_COMM_FORMAT_I2S_MSB,
		.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
		.dma_buf_count = 8,
		.dma_buf_len = 256,
		.use_apll = false,
		.tx_desc_auto_clear = true,
		.fixed_mclk = 0
	};

	i2s_pin_config_t pin_config = {
		.bck_io_num   = I2S_BCK_IO,
		.ws_io_num    = I2S_WS_IO,
		.data_out_num = I2S_DO_IO,
		.data_in_num  = I2S_PIN_NO_CHANGE
	};

	// ── Driver installeren ─────────────────────────────
	esp_err_t err = i2s_driver_install(
		I2S_PORT,
		&i2s_config,
		0,
		nullptr
	);

	if (err != ESP_OK) {
		Serial.print("I2S install failed: ");
		Serial.println(err);
		while (true); // hard stop, want audio kan niet werken
	}

	// ── Pins instellen ─────────────────────────────────
	err = i2s_set_pin(I2S_PORT, &pin_config);
	if (err != ESP_OK) {
		Serial.print("I2S set pin failed: ");
		Serial.println(err);
		while (true);
	}

	// ── Buffer leegmaken ───────────────────────────────
	i2s_zero_dma_buffer(I2S_PORT);

	Serial.println("I2S ready");
}
#endif