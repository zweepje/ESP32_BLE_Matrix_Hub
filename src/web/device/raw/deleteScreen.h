#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_deleteScreen("/device/raw/deleteScreen", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    long screen = getParamLong(request, "screen");
    device->deleteScreen(screen);
    request->send(200, "text/plain", "OK");
});