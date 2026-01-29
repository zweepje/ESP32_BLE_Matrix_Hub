#include "Display.h"

#include "Adafruit_SSD1306.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR    0x3C



Display::Display() {
	Serial.println("start is available!");
	ipaddress = "x.x.x.x";
	oleddisplay = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
	available = false ;
	if ( CheckDisplay() ){
		oleddisplay->begin(SSD1306_SWITCHCAPVCC, 0x3C);
	};
}


bool Display::CheckDisplay() {

	available = false;

	Wire.begin(8, 9);
	delay(100);
	Wire.beginTransmission(0x3C);
	byte error = Wire.endTransmission();

	if (error == 0) {
		Serial.println("Succes: Scherm gevonden!");
		//
		available = true;
	}
	else if (error == 4) {
		Serial.println("Fout: Onbekende fout op de I2C bus (Check je draden!)");
	}
	else {
		Serial.print("Fout: Geen scherm op 0x3C, error code: ");
		Serial.println(error);
	}
	return available;
}

bool Display::isAvailable() {

	return available;
}

void Display::SetIp( String ip ) {
	ipaddress = ip;
}

void Display::SetStatus( String stat ) {
	status = stat;
}

void Display::display() {

	if ( !available ) { return;}
	wisScherm() ;
}

