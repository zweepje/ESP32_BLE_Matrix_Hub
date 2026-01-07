//
// Created by zweepje on 16-11-2025.
//

#ifndef MYIPIXEL_MAIN_H
#define MYIPIXEL_MAIN_H
#include "iPixelDevice.h"

#define KOOKWEKKER




// forward
class iPixelDevice ;
iPixelDevice *finddevice( String mac ) ;

struct ClientState {
    // Pointer naar het matrix-object dat deze client bestuurt
    iPixelDevice* assignedMatrix = nullptr;
};


#endif //MYIPIXEL_MAIN_H