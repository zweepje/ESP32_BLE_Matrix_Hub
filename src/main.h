//
// Created by zweepje on 16-11-2025.
//

#ifndef MYIPIXEL_MAIN_H
#define MYIPIXEL_MAIN_H
#include "iPixelDevice.h"
#include "utils/webserial.h"

#define KOOKWEKKER


struct TouchButton {
	uint8_t			pin ;
	touch_value_t	threshold ;
	u32_t			firstPress ;
	bool			isOn ;
	bool			thresholdread ;

	TouchButton( uint8_t pin ) {
		this->pin = pin;
		threshold = touchRead( pin ) ;
		firstPress = 0 ;
		isOn = false ;
		thresholdread = false;
	}

	bool check() {

		if ( !thresholdread ) {
			threshold = touchRead(pin);
			thresholdread = true ;
		}
		touch_value_t current = touchRead( pin );
		debugPrintf("current %d  threshold %d\n", (int)current, (int)threshold );
		if ( current > ( threshold * 1.15 ) ) {
			if ( !isOn ) {
				firstPress = millis();	// record starttime
				isOn = true;
				return true ;			// only 1 time!
			}
			isOn = true ;
			return false ; // we already gave this info

		} else {
			isOn = false ;
			firstPress = 0 ;
			return false ;
		}
		return false ;
	}
};


extern TouchButton btnMinutes ;
extern TouchButton btnSeconds ;
extern TouchButton btnStart ;



// forward
class iPixelDevice ;
iPixelDevice *finddevice( String mac ) ;

struct ClientState {
    // Pointer naar het matrix-object dat deze client bestuurt
    iPixelDevice* assignedMatrix = nullptr;
};


#endif //MYIPIXEL_MAIN_H