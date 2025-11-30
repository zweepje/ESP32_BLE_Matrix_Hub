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





bool orgmake_temperature( std::vector<uint8_t>& binaryDataVector, float temperature, String title ) {

    auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    BitmapGFX canvas( *bmp);

    //GifMaker gifMaker ;    //


    canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
    canvas.setTextSize(1);

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
    gifEngine.MakeGif( bmp->getData(), aPalette, numColors );
    gifEngine.CloseGif();
    gifEngine.GetResults(binaryDataVector);
    return true ;

}

bool make_temperature( std::vector<uint8_t>& binaryDataVector, float temperature, String title ) {


    GifMaker gifEngine ;

for ( int i=0; i<4 ; i++ ) {
    auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    BitmapGFX canvas( *bmp);

    canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
    canvas.setTextSize(1);

    canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
    canvas.setTextSize(1);  // 5x7 pixels
    canvas.setTextColor(RED);         // Stel de tekstkleur in op Index 1 (bijv. wit)
    canvas.setCursor( i*2, 14) ;
    canvas.print("abc");


    Serial.printf("adding gif number %d\n", i );

    if ( i==0 ) {
        gifEngine.MakeGif( bmp->getData(), aPalette, numColors );
    }
    else {
        gifEngine.AddGif( bmp->getData() );
    }
}

    gifEngine.CloseGif();
    gifEngine.GetResults(binaryDataVector);

//    creategif( bmp );


/*
    unsigned char* data_ptr = getFileBuffer(pGIF); // De unsigned char pointer
    size_t len = getFileCount(pGIF);               // De lengte van de data

    Serial.printf("there are %d bytes in file buffer\n", len );

    for (int i=0; i<len ; i++ ) {
        Serial.printf("%2x", data_ptr[i] ) ;
    }

    // 1. Cast de unsigned char* naar const char*
    const char* char_data = (const char*)data_ptr;

    // 2. Gebruik de String constructor
    String result_string(char_data, len);
    resultString = result_string;

    Serial.print("De lengte van de String is: ");
    Serial.println(resultString.length());  // print de lengte van de String als getal
    //Serial.println("het resultaat is\n");
    //Serial.println(resultString );
    //free(pImageData);                                              // free image data when frame is added
*/
/*

    //
    // return the result
    //
    unsigned char* received_data = getFileBuffer(pGIF); // De unsigned char pointer
    size_t received_len = getFileCount(pGIF);               // De lengte van de data

    if (fillVectorFromCharPtr(binaryDataVector, received_data, received_len)) {
        // De vector is nu gevuld met de bytes
        Serial.printf("Vector succesvol gevuld, grootte: %u bytes\n", binaryDataVector.size());

        // Roep de functie aan die de vector nodig heeft:
        // this->sendPNG(binaryDataVector);
    } else {
        Serial.println("Fout bij vullen van de vector.");
    }


*/

    return true ;

}