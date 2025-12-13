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

#include "Animation.h"
#include "../png/IndexedBitmap.h"
#include "../png/LetterDraw.h"


const int    BLACK = 0 ;
const int    RED = 1 ;
const int    GREEN = 2 ;
const int    BLUE = 3 ;

uint8_t       aPalette[] = {0x00, 0x00, 0x00,                 // black
                            0xFF, 0x00, 0x00,                 // red
                            0x00, 0xFF, 0x00,                 // green
                            0xFF, 0x00, 0xFF                  // blue
                };                // blue

uint16_t numColors = 4;                                        // number of colors in aPalette (up to 256 possible)




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



IndexedBitmap startBitmap = IndexedBitmap(WIDTH, HEIGHT, 8);



bool make_animated_time( std::vector<uint8_t>& binaryDataVector, String time) {

    auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    BitmapGFX canvas( *bmp);

    Serial.printf("timestring is %s\n", time.c_str() );

    // display temperature
    //canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
    canvas.setFont(NULL); // Gebruik een ingesloten lettertype
    canvas.setTextSize(1);  // 5x7 pixels
    canvas.setTextColor(RED);         // Stel de tekstkleur in op Index 1 (bijv. wit)
    canvas.setCursor( 2, 8) ;
    canvas.print(time);

    Animation anim = Animation();
    anim.MakeAnimation( binaryDataVector, &startBitmap, bmp ) ;

    startBitmap = *bmp ; // copy the bitmap to startbitmap
    return true ;

}

static int counter=0 ;
//
// Maakt een animated gif van een temperatuur die naar een vol scherm scrollt
//
bool make_animated_temperature( std::vector<uint8_t>& binaryDataVector, float temperature, String title ) {

    auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);

    char tempBuffer[6]; // Buffer moet groot genoeg zijn voor "±XX.X\0"

    Serial.printf("Temperature counter is %d\n", temperature, counter );


    // make a debug image
    /*    for ( int x= 0 ; x<32 ; x++ ) {

            bmp->setPixel( x,x, 1);
            bmp->setPixel( 31-x,x, 1);
        }//
        */

    sprintf(tempBuffer, "%d", counter);

    tekenString( *bmp, "323", 1, 12, RED, largefont ) ;
    tekenString( *bmp, "312", 1, 30, BLUE, largefont ) ;


    // display temperature
    //canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
    /*    canvas.setFont(NULL); // Gebruik een ingesloten lettertype
        canvas.setTextSize(1);  // 5x7 pixels
        canvas.setTextColor(RED);         // Stel de tekstkleur in op Index 1 (bijv. wit)
        canvas.setCursor( 4, 8) ;
        canvas.print(temperatureString);

        // display environment
        canvas.setFont(NULL); // Gebruik het standaard 5x7 font
        canvas.setTextSize(1);  // 5x7 pixels
        canvas.setTextColor(GREEN);         // Stel de tekstkleur in op Index 1 (bijv. wit)
        canvas.setCursor( 1, 21 ) ;
        canvas.print(title);
    */
    Animation anim = Animation();
    anim.MakeAnimation( binaryDataVector, &startBitmap, bmp ) ;



    startBitmap = *bmp ; // copy the bitmap to startbitmap
    return true ;

}

//
// Maakt een animated gif van een temperatuur die naar een vol scherm scrollt
//
bool orgmake_animated_temperature( std::vector<uint8_t>& binaryDataVector, float temperature, String title ) {

    auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    BitmapGFX canvas( *bmp);

    // String maken van temperature
    char tempBuffer[6]; // Buffer moet groot genoeg zijn voor "±XX.X\0"
    // dtostrf(float_var, totale_breedte, decimalen, buffer)
    dtostrf(temperature, 4, 1, tempBuffer);
    String temperatureString = tempBuffer;

    Serial.printf("Temperature is %f, string is %s\n", temperature, temperatureString.c_str() );


    // make a debug image
/*    for ( int x= 0 ; x<32 ; x++ ) {

        bmp->setPixel( x,x, 1);
        bmp->setPixel( 31-x,x, 1);
    }//
    */

    //tekenCijfer( *bmp, '1', 2, 14, RED );
    //tekenCijfer( *bmp, '2', 7, 14, GREEN );
    //tekenCijfer( *bmp, '3', 16, 14, BLUE );
    tekenString( *bmp, "323", 1, 12, RED, largefont) ;
    tekenString( *bmp, "312", 1, 30, BLUE, largefont ) ;


    // display temperature
    //canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
/*    canvas.setFont(NULL); // Gebruik een ingesloten lettertype
    canvas.setTextSize(1);  // 5x7 pixels
    canvas.setTextColor(RED);         // Stel de tekstkleur in op Index 1 (bijv. wit)
    canvas.setCursor( 4, 8) ;
    canvas.print(temperatureString);

    // display environment
    canvas.setFont(NULL); // Gebruik het standaard 5x7 font
    canvas.setTextSize(1);  // 5x7 pixels
    canvas.setTextColor(GREEN);         // Stel de tekstkleur in op Index 1 (bijv. wit)
    canvas.setCursor( 1, 21 ) ;
    canvas.print(title);
*/
    Animation anim = Animation();
    anim.MakeAnimation( binaryDataVector, &startBitmap, bmp ) ;



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