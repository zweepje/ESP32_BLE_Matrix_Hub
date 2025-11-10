#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_sendGIF("/device/raw/sendGIF", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    std::vector<uint8_t> data = getParamHex(request, "data");
    device->sendGIF(data);
    request->send(200, "text/plain", "OK");
});