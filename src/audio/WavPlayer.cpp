#include "WavPlayer.h"
#include <LittleFS.h>

#define BLOCK_SAMPLES 256   // mono samples per blok

WavPlayer::WavPlayer(i2s_port_t port)
: i2s_port(port) {}

bool WavPlayer::parseWavHeader(File& file, uint32_t& sampleRate) {
    if (!file || file.size() < 44) return false;

    uint8_t header[44];
    file.read(header, 44);

    // Check "RIFF"
    if (memcmp(header, "RIFF", 4) != 0) return false;
    if (memcmp(header + 8, "WAVE", 4) != 0) return false;

    sampleRate =
        header[24] |
        (header[25] << 8) |
        (header[26] << 16) |
        (header[27] << 24);

    uint16_t bitsPerSample =
        header[34] |
        (header[35] << 8);

    uint16_t channels =
        header[22] |
        (header[23] << 8);

    // We ondersteunen bewust alleen dit:
    if (bitsPerSample != 16 || channels != 1) {
        return false;
    }

    return true;
}

bool WavPlayer::play(const char* path, volatile bool* stopFlag) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println("WAV open failed");
        return false;
    }

    uint32_t sampleRate;
    if (!parseWavHeader(file, sampleRate)) {
        Serial.println("Invalid WAV format");
        file.close();
        return false;
    }

    // Zorg dat I2S exact klopt met de WAV
    i2s_set_clk(
        i2s_port,
        sampleRate,
        I2S_BITS_PER_SAMPLE_16BIT,
        I2S_CHANNEL_STEREO
    );

    int16_t mono_buffer[BLOCK_SAMPLES];
    int16_t stereo_buffer[BLOCK_SAMPLES * 2];


    while (true) {

        if (stopFlag && *stopFlag) {
            Serial.println("Playback aborted");
            break;
        }

        int bytesRead = file.read(
            (uint8_t*)mono_buffer,
            BLOCK_SAMPLES * sizeof(int16_t)
        );

        if (bytesRead <= 0) break;



        if (bytesRead <= 0) break;

        int samples = bytesRead / 2;

        // Mono → stereo
        for (int i = 0; i < samples; i++) {
            int16_t s = mono_buffer[i];
            stereo_buffer[i * 2]     = s;
            stereo_buffer[i * 2 + 1] = s;
        }

        size_t bytesWritten;
        i2s_write(
            i2s_port,
            stereo_buffer,
            samples * 4,
            &bytesWritten,
            portMAX_DELAY
        );
    }

    file.close();

    // Buffer netjes leegmaken
    int16_t silence[64] = {0};
    size_t bw;
    for (int i = 0; i < 4; i++) {
        i2s_write(i2s_port, silence, sizeof(silence), &bw, portMAX_DELAY);
    }

    i2s_zero_dma_buffer(i2s_port);

    return true;
}
