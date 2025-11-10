#pragma once
#include "iPixelDeviceRegistry.h"
#include "web/Helpers.h"

iPixelDevice* getParamIPixelDevice(AsyncWebServerRequest* request, const char* name) {
    String device = getParamString(request, name);
    NimBLEAddress addr(device.c_str(), 0);
    iPixelDevice* dev = getOrCreateDevice(addr);
    if (!dev->connected) throw new std::invalid_argument("Device is (still) connecting");
    return dev;
}