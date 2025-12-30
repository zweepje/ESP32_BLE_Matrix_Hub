//
// Created by zweepje on 30-12-2025.
//

#include "ChunkProcessor.h"

#include "utils/webserial.h"


//
// size is toch in hexnotatie???
//
ChunkProcessor::ChunkProcessor( size_t size) {

    dataBuffer.clear();
    dataBuffer.reserve(size);
    received = 0 ;
    started = true ;


}

//
// received a chunk of data
//
void ChunkProcessor::process( const char *data ) {

    if ( ! started ) {
        return ;
    }
    // 2. Voeg de huidige chunk (data) toe aan de buffer
    size_t len = strlen(data);
    for (size_t i = 0; i < len; i++) {
        dataBuffer.push_back(data[i]);
        received++ ;
    }

}
std::vector<uint8_t> * ChunkProcessor::ending(  ) {

    if ( !started ) {
        return nullptr ;
    }
    started = false ;
    debugPrintf("Received complete PACKAGE. Size %d %d\n", received, dataBuffer.size());
    return &dataBuffer ;
}
