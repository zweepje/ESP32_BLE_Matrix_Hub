
#ifndef WEKKER_DISPLAY_H
#define WEKKER_DISPLAY_H

#include <WString.h>

#include "Adafruit_SSD1306.h"
#include "oleddisplay.h"

class Display {

	String ipaddress;
	String status ;
	bool available ;
	Adafruit_SSD1306 *oleddisplay ;

public:
	Display() ;
	void SetIp( String ip );
	void display() ;
	void SetStatus( String stat );
	bool isAvailable() ;


private:
	bool CheckDisplay() ;
};


#endif //WEKKER_DISPLAY_H