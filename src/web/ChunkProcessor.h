//
// Created by zweepje on 30-12-2025.
//

#ifndef MYIPIXEL_CHUNKPROCESSOR_H
#define MYIPIXEL_CHUNKPROCESSOR_H
#include <vector>
#include <cstring>
#include <string>
#include <cstdint>

class ChunkProcessor {

private:
    std::vector<uint8_t> dataBuffer;
    int received = 0;
    bool started = false;

public:
    ChunkProcessor( size_t size);
    void process( const char *data );
    std::vector<uint8_t> *ending() ;

};


#endif //MYIPIXEL_CHUNKPROCESSOR_H