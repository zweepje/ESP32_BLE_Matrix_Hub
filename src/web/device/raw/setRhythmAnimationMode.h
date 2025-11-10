#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setRhythmAnimationMode("/device/raw/setRhythmAnimationMode", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    long style = getParamLong(request, "style");
    long frame = getParamLong(request, "frame");
    device->setRhythmAnimationMode(
        style,
        frame
    );
    request->send(200, "text/plain", "OK");
});