//
// Created by zweepje on 13-12-2025.
//

#ifndef MYIPIXEL_MYFONT_H
#define MYIPIXEL_MYFONT_H
#include <cstdint>

class FontInfo {

    char    character;
    uint8_t width, height;
    uint8_t xoffset, yoffset;
public:
    FontInfo(char c, uint32_t w, uint32_t h, uint8_t x_o, uint8_t y_o);
};

class MyFont {

    public:
        MyFont( const uint8_t *bm, uint8_t width, FontInfo *fi );

    const uint8_t *bitmap{};
    uint8_t bitmapWidth{};
    FontInfo *fontinfo{};
};


#endif //MYIPIXEL_MYFONT_H