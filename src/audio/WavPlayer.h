#ifndef MYIPIXEL_WAVPLAYER_H
#define MYIPIXEL_WAVPLAYER_H

#include <Arduino.h>
#include <FS.h>
#include "driver/i2s.h"

class WavPlayer {
public:
	WavPlayer(i2s_port_t port = I2S_NUM_0);

	bool play(const char* path);

private:
	i2s_port_t i2s_port;

	bool parseWavHeader(File& file, uint32_t& sampleRate);
};

#endif //MYIPIXEL_WAVPLAYER_H