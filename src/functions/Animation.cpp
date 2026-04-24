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

// ChatGTP oplossing
enum SlideDirection {
    SLIDE_UP,
    SLIDE_DOWN,
    SLIDE_LEFT,
    SLIDE_RIGHT
};

void slideBitmap(
    IndexedBitmap* target,
    IndexedBitmap* from,
    IndexedBitmap* to,
    int progress,
    SlideDirection dir
) {
    int w = target->getWidth();
    int h = target->getHeight();

    progress = std::max(0, std::min(progress, (dir == SLIDE_LEFT || dir == SLIDE_RIGHT) ? w : h));

    switch (dir) {

        case SLIDE_DOWN:
            // oude naar beneden, nieuwe van boven
            from->copyTo(*target, 0, 0, 0, progress, w, h - progress);
            to->copyTo(*target, 0, h - progress, 0, 0, w, progress);
            break;

        case SLIDE_UP:
            // oude naar boven, nieuwe van onder
            from->copyTo(*target, 0, progress, 0, 0, w, h - progress);
            to->copyTo(*target, 0, 0, 0, h - progress, w, progress);
            break;

        case SLIDE_RIGHT:
            // oude naar rechts, nieuwe van links
            from->copyTo(*target, 0, 0, progress, 0, w - progress, h);
            to->copyTo(*target, w - progress, 0, 0, 0, progress, h);
            break;

        case SLIDE_LEFT:
            // oude naar links, nieuwe van rechts
            from->copyTo(*target, progress, 0, 0, 0, w - progress, h);
            to->copyTo(*target, 0, 0, w - progress, 0, progress, h);
            break;
    }
}
//
// helpers:
//
void putin( IndexedBitmap *target, IndexedBitmap *src1, IndexedBitmap *src2, int places ) {
      src1->copyTo( *target, 0, places, 0, 0, 32, 32-places);
      src2->copyTo( *target, 0, 0, 0, 32-places, 32, places);
}

void putinvertical(  IndexedBitmap *target, IndexedBitmap *src1, IndexedBitmap *src2, int places ) {
    src1->copyTo( *target, places, 0, 0,         0, 32-places, 32);
    src2->copyTo( *target, 0,      0, 32-places, 0, places,    32);
}

void putinverticallr(  IndexedBitmap *target, IndexedBitmap *src1, IndexedBitmap *src2, int places ) {
    src1->copyTo( *target, places, 0, 0, 0, 32-places, 32);
    src2->copyTo( *target, 0, 0, 32-places, 0, places, 32);
}

void test( IndexedBitmap *target, IndexedBitmap *src1, IndexedBitmap *src2, int places ) {
    src2->copyTo( *target, 0, 0, 0, 0, 32, 32);

}

Animation::Animation() {

    if ( gifEngine==NULL) {
        gifEngine = new GifMaker();
    }
}


static IndexedBitmap tmpbmp(WIDTH, HEIGHT, 8);

void Animation::MakeGif( std::vector<uint8_t>& binaryDataVector, IndexedBitmap *start ) {

    //Serial.printf("Inside MakeGif\n" );

//    gifEngine->MakeGif( start->getData(), aPalette, numColors, false );
    gifEngine->MakeGif( start->getData(), aPalette, numColors, true, 100000 );
    //Serial.printf("Inside MakeGif 1\n" );
    gifEngine->CloseGif();
    //Serial.printf("Inside MakeGif 2\n" );
    gifEngine->GetResults(binaryDataVector);
    //Serial.printf("Inside MakeGif 3\n" );

}

SlideDirection toDirection(int value) {
    switch (value) {
        case 0: return SLIDE_UP;
        case 1: return SLIDE_DOWN;
        case 2: return SLIDE_LEFT;
        case 3: return SLIDE_RIGHT;
        default: return SLIDE_UP; // fallback
    }
}

void Animation::MakeAnimation( std::vector<uint8_t>& binaryDataVector, IndexedBitmap *start, IndexedBitmap *end ) {

    //gifEngine.MakeGif( bmp->getData(), aPalette, numColors, true, 50 );

    int stepsize = 4 ;
    int steps = 32 / stepsize ;
    int time = 1000 ;   // msec

    int speed = ( time / steps ) ;  // in mSec
    //speed = 1000 ; //debug 1 seconds
    speed /= 10 ;   // adapt to gif standard of 10ms units
    // start with oldbitmap
    gifEngine->MakeGif( start->getData(), aPalette, numColors, true, speed );

    //auto *tmpbmp = new IndexedBitmap(WIDTH, HEIGHT, 8);
    tmpbmp.clear() ;

    int dir = rand() % 4 ;

    for ( int i=stepsize ; i<=32 ; i+=stepsize ) {

        int tspeed ;

        slideBitmap( &tmpbmp, start, end, i, toDirection( dir ) ) ;


        // putin( &tmpbmp, start, end, i );
        //putin( &tmpbmp, start, end, 32-i );
        //putinvertical( &tmpbmp, start, end, i );
        //putinvertical( &tmpbmp, start, end, 32 - i );


        if ( (i+stepsize)  > 32 ) {
            tspeed = 5000; // 50 seconds
            Serial.printf("Temp, setting long time\n" );

        } else {
            tspeed = speed ;
        }
        gifEngine->AddGif( tmpbmp.getData(), tspeed );
    }

    gifEngine->CloseGif();
    gifEngine->GetResults(binaryDataVector);

}