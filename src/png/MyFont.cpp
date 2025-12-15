//
// Created by zweepje on 13-12-2025.
//

#include "Arduino.h"
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
    FontInfo( ',', 2, 10, 36, 19),
    FontInfo( '.', 2, 10, 36, 19),
    FontInfo( ' ', 8, 10, 38, 19)

};

FontInfo LargeFont2Info[] =
{
    FontInfo( '1', 3, 7, 2, 27),
    FontInfo( '2', 5, 7, 6, 27),
    FontInfo( '3', 5, 7, 12, 27),
    FontInfo( '4', 5, 7, 18, 27),
    FontInfo( '5', 5, 7, 24, 27),
    FontInfo( '6', 5, 7, 30, 27),
    FontInfo( '7', 5, 7, 36, 27),
    FontInfo( '8', 5, 7, 42, 27),
    FontInfo( '9', 5, 7, 48, 27),
    FontInfo( '0', 5, 7, 54, 27),
    FontInfo( ':', 1, 7, 60, 27),

};

FontInfo SmallFontInfo[] =
{
    FontInfo( 'a', 5, 7, 0, 6),
    FontInfo( 'b', 5, 7, 5, 6),
    FontInfo( 'c', 5, 7, 10, 6),
    FontInfo( 'd', 5, 7, 15, 6),
    FontInfo( 'e', 5, 7, 20, 6),
    FontInfo( 'f', 3, 7, 25, 6),
    FontInfo( 'g', 5, 7, 30, 6),
    FontInfo( 'h', 5, 7, 35, 6),
    FontInfo( 'i', 1, 7, 40, 6),
    FontInfo( 'j', 3, 7, 45, 6),
    FontInfo( 'k', 5, 7, 50, 6),
    FontInfo( 'l', 1, 7, 55, 6),
    FontInfo( 'm', 5, 7, 60, 6),
    FontInfo( 'n', 5, 7, 65, 6),
    FontInfo( 'o', 5, 7, 70, 6),
    FontInfo( 'p', 5, 7, 75, 6),
    FontInfo( 'q', 5, 7, 80, 6),
    FontInfo( 'r', 5, 7, 85, 6),
    FontInfo( 's', 5, 7, 90, 6),
    FontInfo( 't', 5, 7, 95, 6),
    FontInfo( 'u', 5, 7, 100, 6),
    FontInfo( 'v', 5, 7, 105, 6),
    FontInfo( 'w', 5, 7, 110, 6),
    FontInfo( 'x', 5, 7, 115, 6),
    FontInfo( 'y', 5, 7, 120, 6),
    FontInfo( 'z', 5, 7, 0, 16),


};




int ElementsLargeFont = sizeof( LargeFontInfo ) / sizeof( FontInfo);
int ElementsLargeFont2 = sizeof( LargeFont2Info ) / sizeof( FontInfo);
int ElementsSmallFont = sizeof( SmallFontInfo ) / sizeof( FontInfo);

MyFont largefont( largefontbitmap, 64, LargeFontInfo, ElementsLargeFont );
MyFont largefont2( largefontbitmap, 64, LargeFont2Info, ElementsLargeFont2 );
MyFont smallfont( smallfontbitmap, 128, SmallFontInfo, ElementsSmallFont );




MyFont::MyFont( const uint8_t *bm, uint8_t width, FontInfo *fi, int size ) :
     bitmap(bm),
     bitmapWidth(width),
     fontinfo(fi),
     nElements(size) {
}

FontInfo *MyFont::getFontInfo(char character) {

    Serial.printf("Nelements is %d\n", nElements );

    for ( int i=0 ; i<nElements ; i++ ) {

        if ( character == fontinfo[i].character ) {
            return &fontinfo[i];
        }

    }
    return &LargeFontInfo[0];
}

bool MyFont::bitset( FontInfo *info, int x, int y ) {

    x = x+info->xoffset ;
    y = info->yoffset-y ;

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


