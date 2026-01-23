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
#include "MatrixContext.h"
#include "../png/IndexedBitmap.h"
#include "../png/letterbitmap.h"
#include "../png/LetterDraw.h"
#include "clock/timefunctions.h"
#include "utils/webserial.h"


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



/*
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


*/

// We have to allocate it one time
static IndexedBitmap bmp(WIDTH, HEIGHT, 8);

int tempy( float t ) {

	int y = static_cast<int>( t );
	y += 7 ;
	if ( y<0 ) y=0 ;
	if ( y>31 ) y=31;
	return 31-y;
}


bool make_temp_graph( void* generic_context, std::vector<uint8_t>& binaryDataVector, float kamertemp[24], float buitentemp[24] ) {

	MatrixContext* context = static_cast<MatrixContext*>(generic_context);
	bmp.clear(0) ;

	int nhour = getHuidigUur() ;
	debugPrintf("Uur is nu %d\n", nhour);

	int colorbinnen = 1 ; //red
	int colorbuiten = 3 ; //blue
	int coloraxis = 2 ; // green

	int yas = 31-7 ;
	int xas = 3 ;

	// draw the X-axis
	bmp.drawLine( xas-1, yas, xas+24+1, yas, coloraxis );
	bmp.setPixel( xas+12, yas+1, coloraxis );
	bmp.setPixel( xas+8, yas+1, coloraxis );
	bmp.setPixel( xas+18, yas+1, coloraxis );

	// The Y-axis ( takes whole height )
	bmp.drawLine( xas, 0, xas, 31, coloraxis );
	bmp.setPixel( xas-1, yas -5, coloraxis );
	bmp.setPixel( xas-1, yas +5, coloraxis );
	bmp.setPixel( xas-1, yas -10, coloraxis );
	bmp.setPixel( xas-1, yas -15, coloraxis );
	bmp.setPixel( xas-1, yas -20, coloraxis );


	int y = tempy( kamertemp[0] );
	int by = tempy( buitentemp[0] );
	for ( int i = 1 ; i <= nhour ; i++ ) {

		debugPrintf("kamer Temp %d is %f\n", i, kamertemp[i]);
		int y2 = tempy( kamertemp[i] );
		bmp.drawLine(i+xas, y, i+xas+1, y2, colorbinnen);
		y = y2 ;

		debugPrintf("buiten Temp %d is %f\n", i, buitentemp[i]);
		int by2 = tempy( buitentemp[i] );
		bmp.drawLine(i+xas, by, i+xas+1, by2, colorbuiten);
		by = by2 ;
	}

	Animation anim = Animation();
	anim.MakeAnimation( binaryDataVector, &context->current_bitmap, &bmp ) ;

	// keep the last image for animation
	context->current_bitmap.CopyFromBitmap(bmp);

	return true ;
}


bool make_animated_time( void* generic_context, std::vector<uint8_t>& binaryDataVector, String time) {

    //auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    MatrixContext* context = static_cast<MatrixContext*>(generic_context);
    //
    // Clear the bitmap  ( we kunnen ook background color zetten )
    //
    bmp.clear(0) ;

    Serial.printf("timestring is %s\n", time.c_str() );
    tekenString( bmp, time.c_str(), 1, 15, GREEN, largefont2 ) ;

    Animation anim = Animation();
    anim.MakeAnimation( binaryDataVector, &context->current_bitmap, &bmp ) ;

    // keep the last image for animation
    context->current_bitmap.CopyFromBitmap(bmp);
    return true ;

}


//
// Maakt een animated gif van een temperatuur die naar een vol scherm scrollt
//
bool make_animated_temperature( void* generic_context, std::vector<uint8_t>& binaryDataVector, float temperature, String title ) {

    MatrixContext* context = static_cast<MatrixContext*>(generic_context);
    //
    // Clear the bitmap  ( we kunnen ook background color zetten )
    //
    bmp.clear(0) ;


    // debug stuff
    /*
    char tempBuffer[6]; // Buffer moet groot genoeg zijn voor "±XX.X\0"
    sprintf(tempBuffer, "%d", context->counter);
    Serial.printf("Temperature counter is %d\n", temperature, context->counter );
    context->counter++;


    tekenString( bmp, tempBuffer, 1, 10, RED, largefont ) ;

    tekenString( bmp, "abcd", 1, 10, RED, smallfont ) ;
    tekenString( bmp, "efgh", 1, 20, BLUE, smallfont ) ;
    tekenString( bmp, "ijkl", 1, 30, BLUE, smallfont ) ;

    */

    // String maken van temperature
    char tempBuffer[6]; // Buffer moet groot genoeg zijn voor "±XX.X\0"
    // dtostrf(float_var, totale_breedte, decimalen, buffer)
    dtostrf(temperature, 4, 1, tempBuffer);
    String temperatureString = tempBuffer;
    Serial.printf("Temperature is %f, string is %s\n", temperature, temperatureString.c_str() );

    tekenString( bmp, temperatureString.c_str(), 1, 15, GREEN, largefont ) ;
    tekenString( bmp, title.c_str(), 1,28, BLUE, smallfont ) ;






    // display temperature
    Animation anim = Animation();
    anim.MakeAnimation( binaryDataVector, &context->current_bitmap, &bmp ) ;



    //startBitmap = *bmp ; // copy the bitmap to startbitmap
    context->current_bitmap.CopyFromBitmap(bmp);
    //context->current_bitmap = *bmp ;
    return true ;

}


bool make_clock( std::vector<uint8_t>& binaryDataVector, int hour, int min, int seconds ) {

    const float PI_F = 3.1415926535f;
    //const float DEG_TO_RAD = PI_F / 180.0f;
    Serial.printf("make_clock was called %d %d %d seconds\n", hour, min, seconds );
    bmp.clear(0) ;

    float angle = seconds * 6.0f * static_cast<float>((PI / 180.0f));
    float centerX = 15.5 ;
    float centerY = 15.5 ;
    float length = 13.0 ;

    // 2. Bereken het eindpunt
    int x1 = round(centerX + length * sin(angle));
    int y1 = round(centerY - length * cos(angle));

    bmp.drawLine(centerX, centerY, x1, y1, 1);

    // minutes
    angle = min * 6.0f * static_cast<float>((PI / 180.0f));
    length = 10.0f;
    x1 = round(centerX + length * sin(angle));
    y1 = round(centerY - length * cos(angle));
    bmp.drawLine(centerX, centerY, x1, y1, 2);

    // hour
    // hours % 12 zorgt dat 13u, 14u etc. gewoon als 1u, 2u worden behandeld.
    float hoursDecimal = static_cast<float>(hour % 12) + (static_cast<float>(min) / 60.0f);
    angle = hoursDecimal * 30.0f * (PI_F / 180.0f);
    //angle = hour * 6.0f * static_cast<float>((PI / 180.0f));
    length = 7.0f;
    x1 = round(centerX + length * sin(angle));
    y1 = round(centerY - length * cos(angle));
    bmp.drawLine(centerX, centerY, x1, y1, 2);



    //tekenString( bmp, timeStr.c_str(), 1, 15, GREEN, largefont2 ) ;
    Animation anim = Animation();
    anim.MakeGif( binaryDataVector, &bmp ) ;
    Serial.printf("make_clock exiting\n" );


// todo: oude bitmap bewaren als we gaan animeren?


    return true;
}


bool make_kooktime( void* generic_context, std::vector<uint8_t>& binaryDataVector, String timeStr ) {

	timeStr="12";

    Serial.printf("make_kooktime was called %s\n", timeStr.c_str() );
    //GifMaker gifEngine ;
    MatrixContext* context = static_cast<MatrixContext*>(generic_context);
    bmp.clear(0) ;
    tekenString( bmp, timeStr.c_str(), 1, 15, GREEN, bignumbersfont ) ;
    Animation anim = Animation();
    anim.MakeGif( binaryDataVector, &bmp ) ;
    return true ;
}


bool ORGmake_kooktime( void* generic_context, std::vector<uint8_t>& binaryDataVector, String timeStr ) {

	Serial.printf("make_kooktime was called %s\n", timeStr.c_str() );
	//GifMaker gifEngine ;
	MatrixContext* context = static_cast<MatrixContext*>(generic_context);
	bmp.clear(0) ;
	tekenString( bmp, timeStr.c_str(), 1, 15, GREEN, largefont2 ) ;
	Animation anim = Animation();
	anim.MakeGif( binaryDataVector, &bmp ) ;
	return true ;
}


/*
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
*/