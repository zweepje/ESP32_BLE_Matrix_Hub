//
// Created by zweepje on 13-12-2025.
//

#include "MyFont.h"

#include "letterbitmap.h"


FontInfo LargeFontInfo[] =
{
    FontInfo( 'a', 4, 5, 1, 2)


};

MyFont LargeFont( cijfers, 64, LargeFontInfo );


int ElementsLargeFont = sizeof( LargeFontInfo ) / sizeof( FontInfo);

MyFont::MyFont( const uint8_t *bm, uint8_t width, FontInfo *fi ) :
     bitmap(bm),
     bitmapWidth(width),
     fontinfo(fi) {
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


