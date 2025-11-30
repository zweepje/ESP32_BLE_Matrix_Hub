//
// Created by zweepje on 29-11-2025.
//

#ifndef MYIPIXEL_GIFMAKER_H
#define MYIPIXEL_GIFMAKER_H
#include <Arduino.h>
#include "png/IndexedBitmap.h"
#include <cstdint>   // Voor uint8_t en size_t
#include <cstring>   // Voor memcpy en memset
#include <algorithm> // Voor std::min en std::max
#include <vector>

#include "cgif/cgif.h"


class GifMaker {

public:
        GifMaker();
        ~GifMaker();
        bool MakeGif( uint8_t *bmp, uint8_t palette[], int numberofcolors ) ;
        bool AddGif( uint8_t *data );
        void CloseGif();
        bool GetResults( std::vector<uint8_t>& binaryDataVector );

private:
        CGIF*                   pGIF;                                          // struct containing the GIF
        CGIF_Config             gConfig;                                        // global configuration parameters for the GIF
        CGIF_FrameConfig        fConfig;
        uint8_t                 *aPalette ;
        int                     numberofcolors ;
};


#endif //MYIPIXEL_GIFMAKER_H