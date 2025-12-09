//
// Created by zweepje on 8-12-2025.
//

#include "Arduino.h"
#include "LetterDraw.h"
#include "IndexedBitmap.h"
#include "letterbitmap.h"


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

bool bitset( const uint8_t *bm, int x, int y) {

    int byteindex = (x/8)+y*8;
    int xx = x % 8;
    uint8_t mask = 1<<(7-xx);
    uint8_t bit = bm[byteindex] & mask;

    //Serial.printf("acces %d, %d  div %d:", x, y, xx );
    //Serial.printf("byte is %02x, mask is %02x", bm[byteindex], mask );

    return ( bit==0 ? false : true );
}

/**
 * Tekent een enkel cijfer uit de bron-bitmap naar de doel-canvas.
 * @param canvas: De BitmapGFX instantie om op te tekenen (je doel-bitmap).
 * @param cijferChar: Het te tekenen cijfer als karakter (bijv. '5').
 * @param startX: X-coördinaat op de canvas waar het cijfer moet beginnen.
 * @param startY: Y-coördinaat op de canvas waar het cijfer moet beginnen.
 * @param kleur: De kleur om de pixels mee te tekenen (meestal 1 voor 'aan'/wit).
 */
void tekenCijfer(IndexedBitmap& bmp, char cijferChar, int startX, int startY, uint16_t kleur) {
    Serial.println("TekenCijfer:");

    if (cijferChar < '0' || cijferChar > '9') {
        return; // Alleen cijfers ondersteund
    }

    // Index is 0 voor '0', 1 voor '1', etc.
    int index = cijferChar - '0';
    const CharInfo& info = letterinfo[index];

    // De offset in de data-array
    uint32_t byte_offset = 0;

    // Loop door elke rij van het karakter (y-as)
    //for (int y = 0; y < info.height; y++) {
    for (int y = info.height -1 ; y>=0 ; y--) {

        for ( int x=0 ; x< info.width; x++ ) {

            if ( bitset( cijfers, x+info.x, info.y - y) ) {
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

    void tekenString( IndexedBitmap& bmp, char *cijferStr, int startX, int startY, uint16_t kleur) {

        for ( int i = 0 ; i < strlen(cijferStr) ; i++) {

            char cijfer = cijferStr[i];

            tekenCijfer( bmp, cijfer, startX, startY, kleur) ;

            int index = cijfer - '0';
            const CharInfo& info = letterinfo[index];
            startX += 1 + info.width;
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

