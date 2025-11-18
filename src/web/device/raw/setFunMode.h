#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setFunMode("/device/raw/setFunMode", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    bool funMode = getParamBool(request, "funMode");
    device->setFunMode(funMode);
    request->send(200, "text/plain", "OK");
});