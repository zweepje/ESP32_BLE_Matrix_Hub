
#include "MatrixContext.h"

#include <cstdint>
//
// Created by zweepje on 14-12-2025.
//
// 🖼️ De State/Context structuur

MatrixContext::MatrixContext(size_t w, size_t h, uint8_t bpp )
    : current_bitmap(w, h, bpp) {
    counter = 0;
}
