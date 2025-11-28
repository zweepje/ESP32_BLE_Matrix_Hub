//
// Created by zweepje on 20-11-2025.
//
#include <Arduino.h>

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <memory>

#include "cBitmap.h"
#include "cgif/cgif.h"

// --- 1. CONFIGURATIE ---
const unsigned int WIDTH = 32;
const unsigned int HEIGHT = 32;

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

uint8_t       aPalette[] = {0xFF, 0x00, 0x00,                 // red
                            0x00, 0xFF, 0x00,                 // green
                            0x00, 0x00, 0xFF};                // blue
uint16_t numColors = 3;                                        // number of colors in aPalette (up to 256 possible)

// --- 3. HOOFDLOGICA ---
bool generateAndSavePng(String& resultString, const char* filename) {

    cBitmap *bmp;
    Serial.printf("in generateAndSavePng\n" );
     bmp = new cBitmap(WIDTH, HEIGHT);
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






    CGIF_Config     gConfig;                                        // global configuration parameters for the GIF
    CGIF*          pGIF;                                          // struct containing the GIF
    CGIF_FrameConfig   fConfig;


    // nu naar gif converteren
    // path niet meer nodig!!!!
    std::string path_str = "example_cgif.gif";
    char* mutable_path = const_cast<char*>(path_str.c_str());


    initGIFConfig(&gConfig, mutable_path, WIDTH, HEIGHT, aPalette, numColors);
    Serial.printf("initGIFConfig\n" );

    pGIF = cgif_newgif(&gConfig);
    Serial.printf("cgif_newgif\n" );

    // add frame to GIF
    initFrameConfig(&fConfig, bmp->bitmap);                         // initialize the frame-configuration
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
    return true ;

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



