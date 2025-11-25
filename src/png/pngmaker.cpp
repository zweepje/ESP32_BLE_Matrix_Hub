//
// Created by zweepje on 20-11-2025.
//
#include <Arduino.h>

#include <iostream>
#include <vector>
#include <cstdint>
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

uint8_t       aPalette[] = {0xFF, 0x00, 0x00,                 // red
                            0x00, 0xFF, 0x00,                 // green
                            0x00, 0x00, 0xFF};                // blue
uint16_t numColors = 3;                                        // number of colors in aPalette (up to 256 possible)

// --- 3. HOOFDLOGICA ---
void generateAndSavePng(const char* filename) {

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
            bmp->setPixel( x, y, 1 ); // Donkerblauw
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
        bmp->setPixel(  15, i, 3); // Groen
    }
    Serial.printf("lijn gemaakt\n" );

    CGIF_Config     gConfig;                                        // global configuration parameters for the GIF

    // nu naar gif converteren
    std::string path_str = "example_cgif.gif";
    char* mutable_path = const_cast<char*>(path_str.c_str());
    initGIFConfig(&gConfig, mutable_path, WIDTH, HEIGHT, aPalette, numColors);
    CGIF*          pGIF;                                          // struct containing the GIF
    pGIF = cgif_newgif(&gConfig);

















   Serial.printf("Vrije Heap (na PNG-encode): %u bytes\n", ESP.getFreeHeap());
 //   if (error) {
 //       Serial.printf("PNG-encoder fout %u: %s\n", error, lodepng_error_text(error));
 //   } else {
 //       Serial.printf("Succesvol opgeslagen als %s (%u x %u pixels).\n", filename, WIDTH, HEIGHT);
 //   }
}

void setuppng() {

    generateAndSavePng("poep") ;

}



