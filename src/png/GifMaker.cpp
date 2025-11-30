//
// Created by zweepje on 29-11-2025.
//

#include "GifMaker.h"


const unsigned int WIDTH = 32;
const unsigned int HEIGHT = 32;



bool fillVectorFromCharPtr(std::vector<uint8_t>& outputVector, const unsigned char* data_ptr, size_t len) {
    if (data_ptr == nullptr || len == 0) {
        Serial.printf("len = 0 or data = NULL %u, %u\n", len, data_ptr );
        outputVector.clear();
        return false;
    }
    Serial.printf("len, data %u, %u\n", len, data_ptr );

    // alloceer databuffer
    outputVector.resize(len);
    memcpy(outputVector.data(), (const void*)data_ptr, len);
    return true;
}


static void initGIFConfig(CGIF_Config* pConfig, char* path, uint16_t width, uint16_t height, uint8_t* pPalette, uint16_t numColors) {

    memset(pConfig, 0, sizeof(CGIF_Config));
    pConfig->width                   = width;
    pConfig->height                  = height;
    pConfig->pGlobalPalette          = pPalette;
    pConfig->numGlobalPaletteEntries = numColors;
    pConfig->path                    = path;
    pConfig->attrFlags               = CGIF_ATTR_IS_ANIMATED;

}

static void initFrameConfig(CGIF_FrameConfig* pConfig, uint8_t* pImageData) {
    memset(pConfig, 0, sizeof(CGIF_FrameConfig));
    pConfig->pImageData = pImageData;
}

static void initFrameConfig(CGIF_FrameConfig* pConfig, uint8_t* pImageData, uint16_t delay) {
    memset(pConfig, 0, sizeof(CGIF_FrameConfig));
    pConfig->pImageData = pImageData;
    pConfig->delay      = delay;
}



    GifMaker::GifMaker() {

        // create the filebuffer here
        Serial.printf("Constructor GifMaker\n" );
    }

    GifMaker::~GifMaker() {

        // free the filebuffer here
        Serial.printf("Destructor GifMaker\n" );
    }

bool GifMaker::MakeGif( uint8_t *data, uint8_t palette[], int numcol ) {

    aPalette = palette;
    numberofcolors = numcol ;
    std::string path_str = "example_cgif.gif";
    char* mutable_path = const_cast<char*>(path_str.c_str());


    initGIFConfig(&gConfig, mutable_path, WIDTH, HEIGHT, aPalette, numberofcolors);
    Serial.printf("initGIFConfig\n" );

    pGIF = cgif_newgif(&gConfig);
    Serial.printf("cgif_newgif was called\n" );

    // add frame to GIF
    initFrameConfig(&fConfig, data, 100 );                         // initialize the frame-configuration

    Serial.printf("initFrameConfig was called\n" );

    if ( pGIF==NULL ) {
        Serial.printf("pGIF = NULL\n" );
        return false ;
    }


    cgif_addframe(pGIF, &fConfig);                                 // add a new frame to the GIF
    Serial.printf("cgif_addframe called.\n" );

    return true ;
}

bool GifMaker::AddGif( uint8_t *data ) {

    initFrameConfig(&fConfig, data, 100 );
    return cgif_addframe(pGIF, &fConfig );


}


void GifMaker::CloseGif() {

    cgif_close(pGIF);
    Serial.printf("cgif_close(pGIF) called.\n" );

}

bool GifMaker::GetResults( std::vector<uint8_t>& binaryDataVector ) {

        unsigned char* received_data = getFileBuffer(pGIF); // De unsigned char pointer
        size_t received_len = getFileCount(pGIF);               // De lengte van de data

        Serial.printf("Vector data at %u, size is %u bytes\n", received_data, received_len );


        if (fillVectorFromCharPtr(binaryDataVector, received_data, received_len)) {
            // De vector is nu gevuld met de bytes
            Serial.printf("Vector succesvol gevuld, grootte: %u bytes\n", binaryDataVector.size());

            // Roep de functie aan die de vector nodig heeft:
            // this->sendPNG(binaryDataVector);
        } else {
            Serial.println("Fout bij vullen van de vector.");
        }


        return true;
    }
