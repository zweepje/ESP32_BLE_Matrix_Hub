#pragma once
#include "web/Endpoint.h"
#include "web/Helpers.h"
#include "web/device/Helpers.h"

Endpoint device_raw_setClockMode("/device/raw/setClockMode", HTTP_GET, [](AsyncWebServerRequest* request) {
    iPixelDevice* device = getParamIPixelDevice(request, "device");
    long style = getParamLong(request, "style");
    long dayOfWeek = getParamLong(request, "dayOfWeek");
    long year = getParamLong(request, "year");
    long month = getParamLong(request, "month");
    long day = getParamLong(request, "day");
    bool showDate = getParamBool(request, "showDate");
    bool format24 = getParamBool(request, "format24");
    device->setClockMode(
        style,
        dayOfWeek,
        year,
        month,
        day,
        showDate,
        format24
    );
    request->send(200, "text/plain", "OK");
});