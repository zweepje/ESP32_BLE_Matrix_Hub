#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

bool displayAvailable = false;

void initdisplay() {
}

void wisScherm() {
	display.clearDisplay(); // Maakt de buffer in het geheugen leeg
	display.display();      // Stuurt de lege buffer naar het fysieke scherm
}


// FUNCTIE 1: Check of het scherm fysiek reageert
bool checkDisplay() {
	// Probeer verbinding te maken
	Wire.beginTransmission(0x3C);
	byte error = Wire.endTransmission();

	if (error == 0) {
		Serial.println("Succes: Scherm gevonden!");
		//
		displayAvailable = true;
		return true;
	}
	else if (error == 4) {
		Serial.println("Fout: Onbekende fout op de I2C bus (Check je draden!)");
	}
	else {
		Serial.print("Fout: Geen scherm op 0x3C, error code: ");
		Serial.println(error);
	}
	return false;
}

// 2. Functie om tekst te schrijven
// x, y: positie (0,0 is linksboven)
// grootte: 1 is klein, 2 is gemiddeld/kookwekker-stijl
void schrijfTekst(String tekst, int x, int y, int grootte) {
	// We wissen niet het hele scherm hier, zodat je meerdere regels kunt schrijven.
	// Wil je een schone lei? Roep dan eerst wisScherm() aan.

	display.setTextSize(grootte);
	display.setTextColor(SSD1306_WHITE);
	display.setCursor(x, y);
	display.print(tekst);

	display.display(); // Belangrijk: zonder dit zie je niks veranderen!
}

