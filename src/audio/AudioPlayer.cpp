#include "AudioPlayer.h"
#include "WavPlayer.h"

AudioPlayer::AudioPlayer()
: taskHandle(nullptr),
  stopRequested(false),
  playing(false) {
}

bool AudioPlayer::isPlaying() {
	return playing;
}

void AudioPlayer::startPlay(const char* filename) {

	// Stop huidige playback als die loopt
	stopCurrent();

	// Bestandsnaam opslaan
	strncpy(currentFile, filename, sizeof(currentFile) - 1);
	currentFile[sizeof(currentFile) - 1] = 0;

	stopRequested = false;
	playing = true;

	// Start audio task
	xTaskCreatePinnedToCore(
		audioTask,
		"AudioTask",
		4096,
		this,
		1,
		&taskHandle,
		0   // core 0 (laat UI op core 1)
	);
}

void AudioPlayer::stopCurrent() {
	if (taskHandle) {
		stopRequested = true;

		// Wacht tot task zichzelf opruimt
		while (taskHandle != nullptr) {
			vTaskDelay(1);
		}
	}
}

void AudioPlayer::audioTask(void* param) {
	AudioPlayer* self = static_cast<AudioPlayer*>(param);

	WavPlayer player;

	player.play(
		self->currentFile,
		&self->stopRequested   // 👈 kleine uitbreiding
	);

	self->playing = false;
	self->taskHandle = nullptr;

	vTaskDelete(nullptr);
}

void AudioPlayer::stop() {
	stopCurrent();
}
