//
// Created by zweepje on 13-12-2025.
//

#include "MyFont.h"

#include "letterbitmap.h"


FontInfo LargeFontInfo[] =
{
    FontInfo( '1', 3, 9, 0, 9),
    FontInfo( '2', 8, 9, 9, 9),
    FontInfo( '3', 8, 9, 18, 9),
    FontInfo( '4', 8, 9, 27, 9),
    FontInfo( '5', 8, 9, 36, 9),
    FontInfo( '6', 8, 9, 45, 9),
    FontInfo( '7', 8, 9, 0, 19),
    FontInfo( '8', 8, 9, 9, 19),
    FontInfo( '9', 8, 9, 18, 19),
    FontInfo( '0', 8, 9, 27, 19),
    FontInfo( ',', 8, 9, 36, 20)
};

MyFont largefont( largefontbitmap, 64, LargeFontInfo );


int ElementsLargeFont = sizeof( LargeFontInfo ) / sizeof( FontInfo);

MyFont::MyFont( const uint8_t *bm, uint8_t width, FontInfo *fi ) :
     bitmap(bm),
     bitmapWidth(width),
     fontinfo(fi) {
}

FontInfo *MyFont::getFontInfo(char character) {

    for ( int i=0 ; i<ElementsLargeFont ; i++ ) {

        if ( character == LargeFontInfo[i].character ) {
            return &LargeFontInfo[i];
        }

    }
    return &LargeFontInfo[0];
}

bool MyFont::bitset( FontInfo *info, int x, int y ) {

    x = x+info->xoffset ;
    y = y+info->yoffset ;

    int byteindex = (x/8)+y*(bitmapWidth/8);
    int xx = x % 8;
    uint8_t mask = 1<<(7-xx);
    uint8_t bit = bitmap[byteindex] & mask;

    //Serial.printf("acces %d, %d  div %d:", x, y, xx );
    //Serial.printf("byte is %02x, mask is %02x", bm[byteindex], mask );

    return ( bit==0 ? false : true );

}





FontInfo::FontInfo(char c, uint32_t w, uint32_t h, uint8_t x_o, uint8_t y_o)
        : character(c),
          width(w),
          height(h),
          xoffset(x_o),
          yoffset(y_o)
{
    // De body van de constructor is leeg omdat we de
    // leden in de initialisatielijst hebben geïnitialiseerd (dit is efficiënt).
}


