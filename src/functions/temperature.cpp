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
#include <Fonts/FreeSans9pt7b.h> // Voorbeeld font


const int    BLACK = 0 ;
const int    RED = 1 ;
const int    GREEN = 2 ;
const int    BLUE = 3 ;

extern uint8_t       aPalette[] ;                // blue
extern uint16_t      numColors;                                        // number of colors in aPalette (up to 256 possible)



const unsigned int WIDTH = 32;
const unsigned int HEIGHT = 32;

extern CGIF*          pGIF;                                          // struct containing the GIF



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



static void initGIFConfig(CGIF_Config* pConfig, char* path, uint16_t width, uint16_t height, uint8_t* pPalette, uint16_t numColors) {

    memset(pConfig, 0, sizeof(CGIF_Config));
    pConfig->width                   = width;
    pConfig->height                  = height;
    pConfig->pGlobalPalette          = pPalette;
    pConfig->numGlobalPaletteEntries = numColors;
    pConfig->path                    = path;
}

static void initFrameConfig(CGIF_FrameConfig* pConfig, uint8_t* pImageData) {
    memset(pConfig, 0, sizeof(CGIF_FrameConfig));
    pConfig->pImageData = pImageData;
}


bool creategif( IndexedBitmap *bmp ) {


    CGIF_Config     gConfig;                                        // global configuration parameters for the GIF
    CGIF_FrameConfig   fConfig;


    // nu naar gif converteren
    // path niet meer nodig!!!!
    std::string path_str = "example_cgif.gif";
    char* mutable_path = const_cast<char*>(path_str.c_str());


    initGIFConfig(&gConfig, mutable_path, WIDTH, HEIGHT, aPalette, 4);
    Serial.printf("initGIFConfig\n" );

    pGIF = cgif_newgif(&gConfig);
    Serial.printf("cgif_newgif\n" );

    // add frame to GIF
    initFrameConfig(&fConfig, bmp->getData() );                         // initialize the frame-configuration
    Serial.printf("read data at: %d\n", bmp->getData() );

    Serial.printf("initFrameConfig\n" );

    if ( pGIF==NULL ) {
        Serial.printf("pGIF = NULL\n" );
        return false ;
    }


    cgif_addframe(pGIF, &fConfig);                                 // add a new frame to the GIF
    Serial.printf("cgif_addframe called.\n" );


    cgif_close(pGIF);
    Serial.printf("cgif_close(pGIF) called.\n" );

    return true ;

}




bool make_temperature( String& resultString, float temperature, String title ) {

    auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    BitmapGFX canvas( *bmp);

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


    creategif( bmp );



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

    return true ;



}