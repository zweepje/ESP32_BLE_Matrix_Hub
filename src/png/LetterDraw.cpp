//
// Created by zweepje on 8-12-2025.
//

#include "Arduino.h"
#include "LetterDraw.h"
#include "IndexedBitmap.h"
//#include "letterbitmap.h"
#include "MyFont.h"




/**
 * Tekent een enkel cijfer uit de bron-bitmap naar de doel-canvas.
 * @param canvas: De BitmapGFX instantie om op te tekenen (je doel-bitmap).
 * @param cijferChar: Het te tekenen cijfer als karakter (bijv. '5').
 * @param startX: X-coördinaat op de canvas waar het cijfer moet beginnen.
 * @param startY: Y-coördinaat op de canvas waar het cijfer moet beginnen.
 * @param kleur: De kleur om de pixels mee te tekenen (meestal 1 voor 'aan'/wit).
 */
void tekenCijfer(IndexedBitmap& bmp, char cijferChar, int startX, int startY, uint16_t kleur, MyFont font) {
    //Serial.printf("TekenCijfer: <%c>", cijferChar );
    FontInfo *info = font.getFontInfo( cijferChar );
    //Serial.printf(" info: <%c> at: %d, %d\n", info->character, info->xoffset, info->yoffset);


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


//
// deprecated
//
void tekenString( IndexedBitmap& bmp, const char *cijferStr, int startX, int startY, uint16_t kleur, MyFont font ) {
#pragma message("LET OP: Gebruik voortaan nieuwetekenString")

    for ( int i = 0 ; i < strlen(cijferStr) ; i++) {

        char cijfer = cijferStr[i];
        tekenCijfer( bmp, cijfer, startX, startY, kleur, font ) ;

        FontInfo *info = font.getFontInfo( cijfer );
        startX += 1 + info->width;
    }
}


void tekenString( IndexedBitmap& bmp, const char *string, int startX, int startY, uint16_t kleur, stringSpec &spec ) {

	for ( int i = 0 ; i < strlen(string) ; i++) {

		char cijfer = string[i];
		tekenCijfer( bmp, cijfer, startX, startY, kleur, *spec.font ) ;
		FontInfo *info = spec.font->getFontInfo( cijfer );
		startX += spec.spacing + info->width;
	}
}




int  calcSize( const char *string, stringSpec &spec ) {

	int len = 0 ;

	for ( int i=0 ; i<strlen(string) ; i++ ) {
		char karakter = string[i];
		FontInfo *info = spec.font->getFontInfo( karakter );
		len += info->width ;
		if ( i<strlen(string) -1 ) {
			len += spec.spacing;
		}
	}

	return len;
}