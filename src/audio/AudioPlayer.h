#pragma once

#include <Arduino.h>

class AudioPlayer {
public:
	AudioPlayer();

	void startPlay(const char* filename);
	bool isPlaying();
	void stop();
private:
	static void audioTask(void* param);
	void stopCurrent();

	TaskHandle_t taskHandle;
	volatile bool stopRequested;
	volatile bool playing;

	char currentFile[64];
};
