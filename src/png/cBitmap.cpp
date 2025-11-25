//
// Created by zweepje on 23-11-2025.
//

#include "cBitmap.h"
#include <Arduino.h>

//
// Bitmap with colorindex  1 byte/pixel
//
cBitmap::cBitmap( int w, int h) {
    width = w;
    height = h;

    bitmap = new uint8_t[width * height * 1];
    if ( bitmap==NULL ) {
        Serial.printf("bitmap alloc failed\n" );

    }
}

void cBitmap::setPixel( int x, int y, uint8_t colorindex ) {

    //Serial.printf("setPixel at %d, %d\n", x, y  );

    if (x >= width || y >= height) return;

    // Index berekenen: (Y * Breedte + X) * 4 (voor RGBA)
    size_t index = 1 * (y * width + x);

    bitmap[index] = colorindex;
    //Serial.printf("exit setPixel\n");

}
