//
// Created by zweepje on 23-11-2025.
//

#ifndef MYIPIXEL_CBITMAP_H
#define MYIPIXEL_CBITMAP_H
#include <cstdint>


class cBitmap {

public:
    int width, height;
    uint8_t *bitmap ;

public:
    cBitmap( int w, int h );
    void setPixel( int x, int y, uint8_t colorindex) ;
};


#endif //MYIPIXEL_CBITMAP_H