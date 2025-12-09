//
// Created by zweepje on 8-12-2025.
//

#ifndef MYIPIXEL_LETTERDRAW_H
#define MYIPIXEL_LETTERDRAW_H
#include "IndexedBitmap.h"

void tekenCijfer(IndexedBitmap& canvas, char cijferChar, int startX, int startY, uint16_t kleur) ;
void tekenString( IndexedBitmap& bmp, char *cijferStr, int startX, int startY, uint16_t kleur) ;




#endif //MYIPIXEL_LETTERDRAW_H