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




// We have to allocate it one time
static IndexedBitmap bmp(WIDTH, HEIGHT, 8);



bool make_animated_time( void* generic_context, std::vector<uint8_t>& binaryDataVector, String time) {

    //auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    MatrixContext* context = static_cast<MatrixContext*>(generic_context);
    //
    // Clear the bitmap  ( we kunnen ook background color zetten )
    //
    bmp.clear(0) ;

    Serial.printf("timestring is %s\n", time.c_str() );
    tekenString( bmp, time.c_str(), 1, 15, GREEN, largefont2 ) ;
/*    String tijdString = time ; //"14:38"; // Voorbeeldtijd
    String urenString;
    String minutenString;
    // Zoek de positie van het scheidingsteken (de dubbelepunt)
    int scheidingstekenIndex = tijdString.indexOf(':');
    if (scheidingstekenIndex != -1) {
        // 1. Uren ophalen (alles vóór de dubbelepunt)
        // De lengte van de uren is gelijk aan de index van het scheidingsteken
        urenString = tijdString.substring(0, scheidingstekenIndex);

        // 2. Minuten ophalen (alles ná de dubbelepunt)
        // Begin op de positie direct na de dubbelepunt (+1) tot het einde
        minutenString = tijdString.substring(scheidingstekenIndex + 1);

        // Optioneel: Trimmen om eventuele onnodige spaties te verwijderen (goede gewoonte)
        urenString.trim();
        minutenString.trim();
    } else {
        // Foutafhandeling als de dubbelepunt niet gevonden is
        Serial.println("Fout: Geen scheidingsteken gevonden in de tijdstring.");
    }

    Serial.print("Uren: "); Serial.println(urenString);     // Output: 14
    Serial.print("Minuten: "); Serial.println(minutenString); // Output: 38

    tekenString( bmp, urenString.c_str(), 1, 15, GREEN, largefont ) ;
    tekenString( bmp, (minutenString.c_str()), 1, 30, GREEN, largefont ) ;
*/



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




bool make_kooktime( void* generic_context, std::vector<uint8_t>& binaryDataVector, String timeStr ) {

    Serial.printf("make_kooktime was called %s\n", timeStr.c_str() );
    GifMaker gifEngine ;
    MatrixContext* context = static_cast<MatrixContext*>(generic_context);
    bmp.clear(0) ;
    tekenString( bmp, timeStr.c_str(), 1, 15, GREEN, largefont2 ) ;
    Animation anim = Animation();
    gifEngine.MakeGif( bmp.getData(), aPalette, numColors  ) ;
    gifEngine.CloseGif();
    gifEngine.GetResults(binaryDataVector);

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