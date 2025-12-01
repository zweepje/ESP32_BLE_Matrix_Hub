//
// Created by zweepje on 29-11-2025.
//
#include <Arduino.h>
#include "png/IndexedBitmap.h"
#include <cstdint>   // Voor uint8_t en size_t
#include <cstring>   // Voor memcpy en memset
#include <algorithm> // Voor std::min en std::max
#include "cgif/cgif.h"
#include <Adafruit_GFX.h>
#include <vector>
#include <Fonts/FreeSans9pt7b.h> // Voorbeeld font
#include "png/GifMaker.h"
#include "temperature.h"

#include "../png/IndexedBitmap.h"


const int    BLACK = 0 ;
const int    RED = 1 ;
const int    GREEN = 2 ;
const int    BLUE = 3 ;

extern uint8_t       aPalette[] ;                // blue
extern uint16_t      numColors;                                        // number of colors in aPalette (up to 256 possible)



const unsigned int WIDTH = 32;
const unsigned int HEIGHT = 32;




class BitmapGFX : public Adafruit_GFX {
public:
    // Wijst naar je IndexedBitmap
    IndexedBitmap& bitmap;

    // Constructor van de GFX-klasse aanroepen
    BitmapGFX(IndexedBitmap& bmp)
        : Adafruit_GFX(bmp.getWidth(), bmp.getHeight()), bitmap(bmp) {}

    // 1. De verplichte drawPixel() functie
    // Deze vertelt GFX hoe het een pixel op jouw bitmap moet tekenen
    void drawPixel(int16_t x, int16_t y, uint16_t colorIndex) override {
        // We gaan ervan uit dat de colorIndex die GFX doorgeeft,
        // direct de 8 BPP kleurindex is die jouw bitmap verwacht.
        // We gebruiken uint16_t cast omdat GFX dat vereist.
        bitmap.setPixel(x, y, (uint8_t)colorIndex);
    }

    // 2. De verplichte drawFastVLine() functie (optioneel, maar aanbevolen voor snelheid)
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override {
        for (int16_t i = 0; i < h; i++) {
            drawPixel(x, y + i, color);
        }
    }
} ; // BitmapGFX

void putin( IndexedBitmap *target, IndexedBitmap *src1, IndexedBitmap *src2, int places ) {


    src1->copyTo( *target, 0, places, 0, 0, 32, 32-places);
    src2->copyTo( *target, 0, 0, 0, 32-places, 32, places);
}



IndexedBitmap startBitmap = IndexedBitmap(WIDTH, HEIGHT, 8);
//
// Maakt een animated gif van een temperatuur die naar een vol scherm scrollt
//
bool make_animated_temperature( std::vector<uint8_t>& binaryDataVector, float temperature, String title ) {

    auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    BitmapGFX canvas( *bmp);

    // String maken van temperature
    char tempBuffer[6]; // Buffer moet groot genoeg zijn voor "±XX.X\0"
    // dtostrf(float_var, totale_breedte, decimalen, buffer)
    dtostrf(temperature, 4, 1, tempBuffer);
    String temperatureString = tempBuffer;

    // display temperature
    canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
    canvas.setTextSize(1);  // 5x7 pixels
    canvas.setTextColor(RED);         // Stel de tekstkleur in op Index 1 (bijv. wit)
    canvas.setCursor( 0, 14) ;
    canvas.print(temperatureString);

    // display environment
    canvas.setFont(NULL); // Gebruik het standaard 5x7 font
    canvas.setTextSize(1);  // 5x7 pixels
    canvas.setTextColor(GREEN);         // Stel de tekstkleur in op Index 1 (bijv. wit)
    canvas.setCursor( 1, 21 ) ;
    canvas.print(title);

    GifMaker gifEngine ;
    gifEngine.MakeGif( bmp->getData(), aPalette, numColors, true, 50 );


    int stepsize = 2 ;
    int steps = 32 / stepsize ;
    int time = 1000 ;   // msec

    int speed = ( time / steps ) ;  // in mSec
    speed /= 10 ;   // adapt to gif standard of 10ms units
    // start with oldbitmap
    gifEngine.MakeGif( startBitmap.getData(), aPalette, numColors, true, speed );
    auto *tmpbmp = new IndexedBitmap(WIDTH, HEIGHT, 8);


    for ( int i=stepsize ; i<32 ; i+=stepsize ) {

        int tspeed ;

        putin( tmpbmp, &startBitmap, bmp, i );
        if ( (i+stepsize)  >= 32 ) {
            tspeed = 10000 ; // very long
            Serial.printf("Temp, setting long time\n" );

        } else {
            tspeed = speed ;
        }
        gifEngine.AddGif( tmpbmp->getData(), tspeed );
    }

    gifEngine.CloseGif();
    gifEngine.GetResults(binaryDataVector);

    startBitmap = *bmp ; // copy the bitmap to startbitmap
    return true ;

}





bool make_temperature( std::vector<uint8_t>& binaryDataVector, float temperature, String title ) {

    GifMaker gifEngine ;
    //
    // create the temperature bitamp
    //
    auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    BitmapGFX canvas( *bmp);

    canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
    canvas.setTextSize(1);

    canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
    canvas.setTextSize(1);  // 5x7 pixels
    canvas.setTextColor(RED);         // Stel de tekstkleur in op Index 1 (bijv. wit)
    canvas.setCursor( 2, 14) ;
    canvas.print("abc");

        gifEngine.MakeGif( bmp->getData(), aPalette, numColors );


    gifEngine.CloseGif();
    gifEngine.GetResults(binaryDataVector);

    return true ;

}