//
// Created by zweepje on 2-12-2025.
//

#include "Animation.h"
#include "png/GifMaker.h"

extern uint8_t       aPalette[] ;                // blue
extern uint16_t      numColors;                                        // number of colors in aPalette (up to 256 possible)
const unsigned int WIDTH = 32;
const unsigned int HEIGHT =32 ;

//
// one instance of gifmaker
//
GifMaker *gifEngine = NULL ;

//
// helpers:
//
void putin( IndexedBitmap *target, IndexedBitmap *src1, IndexedBitmap *src2, int places ) {
    src1->copyTo( *target, 0, places, 0, 0, 32, 32-places);
    src2->copyTo( *target, 0, 0, 0, 32-places, 32, places);
}



Animation::Animation() {

    if ( gifEngine==NULL) {
        gifEngine = new GifMaker();
    }
}


void Animation::MakeAnimation( std::vector<uint8_t>& binaryDataVector, IndexedBitmap *start, IndexedBitmap *end ) {

    //gifEngine.MakeGif( bmp->getData(), aPalette, numColors, true, 50 );

    int stepsize = 2 ;
    int steps = 32 / stepsize ;
    int time = 1000 ;   // msec

    int speed = ( time / steps ) ;  // in mSec
    speed /= 10 ;   // adapt to gif standard of 10ms units
    // start with oldbitmap
    gifEngine->MakeGif( start->getData(), aPalette, numColors, true, speed );
    auto *tmpbmp = new IndexedBitmap(WIDTH, HEIGHT, 8);


    for ( int i=stepsize ; i<=32 ; i+=stepsize ) {

        int tspeed ;

        putin( tmpbmp, start, end, i );
        if ( (i+stepsize)  > 32 ) {
            tspeed = 10000 ; // very long
            //Serial.printf("Temp, setting long time\n" );

        } else {
            tspeed = speed ;
        }
        gifEngine->AddGif( tmpbmp->getData(), tspeed );
    }

    gifEngine->CloseGif();
    gifEngine->GetResults(binaryDataVector);

}