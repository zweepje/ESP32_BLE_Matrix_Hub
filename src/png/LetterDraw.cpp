//
// Created by zweepje on 8-12-2025.
//

#include "Arduino.h"
#include "LetterDraw.h"
#include "IndexedBitmap.h"
#include "letterbitmap.h"
#include "MyFont.h"


// De structuur die de metadata van één karakter opslaat
struct CharInfo {
    uint8_t width;              // Breedte van het karakter (bijv. 10)
    uint8_t height;             // Hoogte van het karakter (bijv. 15)
    uint8_t x ;                 // position in bitmap
    uint8_t y ;
};

static CharInfo letterinfo[] = {
    { 8,7,1,1 },
{ 4,9,2,8 },
{ 8,9,7,8 },
{ 8,9,16,8 },
{ 8,9,1,1 },
{ 8,9,1,1 },
{ 8,9,1,1 },
{ 8,9,1,1 },
{ 8,9,1,1 },
{ 8,9,1,1 }

};


// Global of statische lookup tabel voor alle cijfers '0' t/m '9'
extern CharInfo karakterLookup[10];


/**
 * Tekent een enkel cijfer uit de bron-bitmap naar de doel-canvas.
 * @param canvas: De BitmapGFX instantie om op te tekenen (je doel-bitmap).
 * @param cijferChar: Het te tekenen cijfer als karakter (bijv. '5').
 * @param startX: X-coördinaat op de canvas waar het cijfer moet beginnen.
 * @param startY: Y-coördinaat op de canvas waar het cijfer moet beginnen.
 * @param kleur: De kleur om de pixels mee te tekenen (meestal 1 voor 'aan'/wit).
 */
void tekenCijfer(IndexedBitmap& bmp, char cijferChar, int startX, int startY, uint16_t kleur, MyFont font) {
    Serial.printf("TekenCijfer: <%c>", cijferChar );

    FontInfo *info = font.getFontInfo( cijferChar );
    Serial.printf(" info: <%c> at: %d, %d\n", info->character, info->xoffset, info->yoffset);


    // Loop door elke rij van het karakter (y-as)
    //for (int y = 0; y < info.height; y++) {
    for (int y = info->height -1 ; y>=0 ; y--) {

        for ( int x=0 ; x< info->width; x++ ) {

            //if ( bitset( cijfers, x+info.x, info.y - y) ) {
            if ( font.bitset( info, x, y ) ) {
                //Serial.println("TekenCijfer: bit true");
                bmp.setPixel( x+startX, startY-y, kleur);

                // set bit in bitmap
            } else {
                // leave it, transparant!
                //Serial.println("TekenCijfer: bit false");

            }
        }
    }
}

    void tekenString( IndexedBitmap& bmp, const char *cijferStr, int startX, int startY, uint16_t kleur, MyFont font ) {

        for ( int i = 0 ; i < strlen(cijferStr) ; i++) {

            char cijfer = cijferStr[i];
            tekenCijfer( bmp, cijfer, startX, startY, kleur, font ) ;


            FontInfo *info = font.getFontInfo( cijfer );
            startX += 1 + info->width;
        }
    }

    int getTekenLength( char *cijferStr ) {

        int len = 0 ;

        for ( int i = 0 ; i < strlen(cijferStr) ; i++) {

            int index = cijferStr[i] - '0';
            const CharInfo& info = letterinfo[index];
            len = len + 1 + info.width;
        }
        return len;
    }

