#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setRhythmLevelMode("/device/raw/setRhythmLevelMode", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    long style = getParamLong(request, "style");
    int levels[11];
    for (int i = 0; i < 11; ++i)
        levels[i] = getParamLong(request, "l" + char(char('0') + i));
    device->setRhythmLevelMode(
        style,
        levels
    );
    request->send(200, "text/plain", "OK");
});