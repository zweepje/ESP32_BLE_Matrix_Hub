//
// Created by zweepje on 8-12-2025.
//
#ifndef MYIPIXEL_LETTERDRAW_H
#define MYIPIXEL_LETTERDRAW_H
#include "IndexedBitmap.h"
#include "MyFont.h"

struct stringSpec {
	MyFont* font ;
	bool fixed ;		// not implemented (yet)
	int  spacing;
};

int  calcSize( const char *string, stringSpec &spec );
void tekenCijfer(IndexedBitmap& canvas, char cijferChar, int startX, int startY, uint16_t kleur, MyFont font) ;
void tekenString( IndexedBitmap& bmp, const char *cijferStr, int startX, int startY, uint16_t kleur, MyFont font) ;
void tekenString( IndexedBitmap& bmp, const char *string, int startX, int startY, uint16_t kleur, stringSpec &spec );

#endif //MYIPIXEL_LETTERDRAW_H