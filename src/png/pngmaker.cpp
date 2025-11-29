//
// Created by zweepje on 20-11-2025.
//
#include <Arduino.h>

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <memory>

#include "IndexedBitmap.h"
#include "cgif/cgif.h"
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h> // Voorbeeld font

// --- 1. CONFIGURATIE ---
const unsigned int WIDTH = 32;
const unsigned int HEIGHT = 32;

// De bridge-klasse erft van Adafruit_GFX
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

uint8_t       aPalette[] = {0x00, 0x00, 0x00,                 // black
                            0xFF, 0x00, 0x00,                 // red
                            0x00, 0xFF, 0x00,                 // green
                            0xFF, 0x00, 0xFF                  // blue
                };                // blue
uint16_t numColors = 4;                                        // number of colors in aPalette (up to 256 possible)

CGIF*          pGIF;                                          // struct containing the GIF


// --- 3. HOOFDLOGICA ---
bool generateAndSavePng(String& resultString, const char* filename) {

    //cBitmap *bmp;
    Serial.printf("in generateAndSavePng\n" );
    auto *bmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    Serial.printf("image allocated\n" );
    if ( bmp==NULL ) {
        Serial.printf("bmp alloc failed\n" );

    }

    // --- Tekenen op de Framebuffer ---

    // Vul de hele achtergrond met een lichtblauw
    for (unsigned int y = 0; y < HEIGHT; y++) {
        for (unsigned int x = 0; x < WIDTH; x++) {
            bmp->setPixel( x, y, 0 ); // Donkerblauw
        }
    }
    Serial.printf("getekend\n" );

    // Teken een rode stip/blok in de hoek
    bmp->setPixel( 5, 5, 2); // Rood
    bmp->setPixel( 6, 5, 2);
    bmp->setPixel( 5, 6, 2);
    bmp->setPixel( 6, 6, 2);

    // Teken een groene lijn
    for (int i = 10; i < 20; i++) {
        bmp->setPixel(  15, i, 1); // Groen
        bmp->setPixel(  i, i, 2); // Groen
        bmp->setPixel(  i, 20, 1); // Groen
    }
    Serial.printf("lijn gemaakt\n" );


    // ------------------------ Adafruit gedonder ---------------------------------------
    BitmapGFX canvas( *bmp);

    // 2. Clear de bitmap eerst
    canvas.fillScreen(0); // Index 0 is zwart/achtergrond

    // 3. Configureer de tekst
    canvas.setFont(&FreeSans9pt7b); // Gebruik een ingesloten lettertype
    canvas.setTextColor(2);         // Stel de tekstkleur in op Index 1 (bijv. wit)
    canvas.setFont(NULL); // Gebruik het standaard 5x7 font
    canvas.setTextSize(1);  // 5x7 pixels

    // 4. Bepaal waar de tekst moet beginnen
    int16_t x1, y1;
    uint16_t w, h;

    String textToPrint="Binnen";
    // Met getTextBounds kun je de positie centreren
    canvas.getTextBounds(textToPrint, 0, 0, &x1, &y1, &w, &h);

    // Stel de cursor in om de tekst te centreren (of op een vaste positie)
    canvas.setCursor(
        (canvas.width() - w) / 2, // X-coördinaat (gecentreerd)
        (canvas.height() + h) / 2 // Y-coördinaat (gecentreerd)
    );
    canvas.setCursor( 2, 21 ) ;

    canvas.print(textToPrint);











    // --------------------- gif encodign --------------------------

    CGIF_Config     gConfig;                                        // global configuration parameters for the GIF
    CGIF*          pGIF;                                          // struct containing the GIF
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

   Serial.printf("Vrije Heap (na PNG-encode): %u bytes\n", ESP.getFreeHeap());
 //   if (error) {
 //       Serial.printf("PNG-encoder fout %u: %s\n", error, lodepng_error_text(error));
 //   } else {
 //       Serial.printf("Succesvol opgeslagen als %s (%u x %u pixels).\n", filename, WIDTH, HEIGHT);
 //   }
    return resultString ;

}

String setuppng() {
    String resultString;
    generateAndSavePng(resultString, "poep") ;
    Serial.print("setuppng De lengte van de String is: ");
    Serial.println(resultString.length());  // print de lengte van de String als getal


    for (int i=0; i<resultString.length() ; i++ ) {
        Serial.printf("%02x", resultString[i] ) ;
    }
    return resultString;
}



