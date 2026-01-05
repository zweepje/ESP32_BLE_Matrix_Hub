//
// Created by zweepje on 2-12-2025.
//

#ifndef MYIPIXEL_ANIMATION_H
#define MYIPIXEL_ANIMATION_H

#include <cstdint>   // Voor uint8_t en size_t
#include <vector>
#include "png/IndexedBitmap.h"

class Animation {

public:
    Animation() ;
    void MakeAnimation( std::vector<uint8_t>& binaryDataVector, IndexedBitmap *start, IndexedBitmap *end );
    void MakeGif(std::vector<uint8_t>& binaryDataVector, IndexedBitmap *start );
};


#endif //MYIPIXEL_ANIMATION_H