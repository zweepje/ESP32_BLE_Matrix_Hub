//
// Created by zweepje on 8-12-2025.
//

#ifndef MYIPIXEL_LETTERDRAW_H
#define MYIPIXEL_LETTERDRAW_H
#include "IndexedBitmap.h"
#include "MyFont.h"

void tekenCijfer(IndexedBitmap& canvas, char cijferChar, int startX, int startY, uint16_t kleur, MyFont font) ;
void tekenString( IndexedBitmap& bmp, const char *cijferStr, int startX, int startY, uint16_t kleur, MyFont font) ;




#endif //MYIPIXEL_LETTERDRAW_H