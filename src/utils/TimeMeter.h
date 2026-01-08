#ifndef TIMEMETER_H
#define TIMEMETER_H
#include <Arduino.h>

class TimeMeter {
private:
	const char* _naam;
	unsigned long _startTime;
	unsigned long _totalTime = 0;
	unsigned long _minTime = 0xFFFFFFFF;
	unsigned long _maxTime = 0;
	int _counter = 0;
	int _limit = 10;

public:
	// Constructor
	TimeMeter(const char* naam, int limit = 10) {
		_naam = naam;
		_limit = limit;
	}

	void start() {
		_startTime = millis();
	}

	void end() {
		unsigned long duration = millis() - _startTime;
		_totalTime += duration;
		_counter++;

		if (duration < _minTime) _minTime = duration;
		if (duration > _maxTime) _maxTime = duration;

		//if (_counter >= _limit) {
		if ( _counter % 10 == 0 ) {
			rapport();
			//reset();
		}
	}

	void rapport() {
		float avg = (float)_totalTime / _counter;
		Serial.println("--- Prestatie Rapport ---");
		Serial.printf("Naam:      %s\n", _naam);
		Serial.printf("Metingen:  %d\n", _counter);
		Serial.printf("Gemiddeld: %.2f mSec\n", avg);
		Serial.printf("Max:       %lu mSec\n", _maxTime);
		Serial.printf("Min:       %lu mSec\n", _minTime);
		Serial.println("-------------------------");
	}

	void reset() {
		_totalTime = 0;
		_counter = 0;
		_minTime = 0xFFFFFFFF;
		_maxTime = 0;
	}
};
#endif