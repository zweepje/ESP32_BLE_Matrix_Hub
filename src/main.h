//
// Created by zweepje on 16-11-2025.
//

#ifndef MYIPIXEL_MAIN_H
#define MYIPIXEL_MAIN_H
#include "iPixelDevice.h"
#include "Display/oleddisplay.h"
#include "utils/webserial.h"

//#define KOOKWEKKER
//const unsigned long repeat_delay = 1000 ;	// 1 sec
//const unsigned long repeat_interval = 333 ;	// 1/3 sec
extern bool debugbuttons;
typedef uint32_t touch_value_t;

struct TouchButton {
	uint8_t			pin ;
	u32_t			firstPress ;
	bool			isOn ;			// one time set
	bool			isPressed  ;		// continous

	bool			repeat ;
	u32_t			interval ;

	TouchButton( uint8_t pin ) {
		this->pin = pin;
		firstPress = 0 ;
		isOn = false ;
		isPressed = false;
		repeat = false ;
		interval = 0 ;
	}

	bool check() {

		int val = digitalRead(pin);

		if ( debugbuttons ) {
			Serial.printf("pin  %d :  %d\n", pin, val );
		}
		if ( val == HIGH ) {
			isPressed = true ;
			if ( !isOn ) {
				repeat = false ;
				interval = 0 ;
				firstPress = millis();	// record starttime
				isOn = true;
				return true ;
			} else {
				if ( ! repeat ) {
					if ( millis()-firstPress > 1000 ) {
						repeat = true ;
						interval = millis() ;
						isOn = true ;
						return true ;
					} else {
						isOn = true ;
						return false ;
					}
				} else {
					if ( millis()-interval > 200 ) {
						interval = millis() ;
						isOn = true ;
						return true ;
					} else {
						isOn = true ;
						return false ;
					}
				}
			}
			isOn = true ;
			return false ; // we already gave this info

		} else {
			repeat = false ;
			isOn = false ;
			isPressed = false ;
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