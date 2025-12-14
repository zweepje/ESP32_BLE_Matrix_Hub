//
// Created by zweepje on 14-12-2025.
//

#ifndef MYIPIXEL_MATRIXCONTEXT_H
#define MYIPIXEL_MATRIXCONTEXT_H
#include <vector>
#include <sys/types.h>
#include "../png/IndexedBitmap.h"

struct MatrixContext {
    // De context bevat nu jouw specifieke bitmap structuur.
    IndexedBitmap current_bitmap;
    int counter ;

    MatrixContext(size_t w, size_t h, uint8_t bpp );

    // Voeg hier eventuele andere benodigde state toe.


};

#endif //MYIPIXEL_MATRIXCONTEXT_H