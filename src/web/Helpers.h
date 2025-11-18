#pragma once
#include <ESPAsyncWebServer.h>
#include <stdexcept>
#include "../Helpers.h"

void requireParam(AsyncWebServerRequest* request, const char* name) {
    if(!request->hasParam(name)) throw new std::invalid_argument("Missing '" + std::string(name) + "' parameter");
};

long getParamLong(AsyncWebServerRequest* request, const char* name) {
    requireParam(request, name);
    return request->getParam(name)->value().toInt();
};

bool getParamBool(AsyncWebServerRequest* request, const char* name) {
    return getParamLong(request, name) > 0;
};

String getParamString(AsyncWebServerRequest* request, const char* name) {
    requireParam(request, name);
    return request->getParam(name)->value();
};

std::vector<uint8_t> getParamHex(AsyncWebServerRequest* request, const char* name) {
    return Helpers::hexStringToVector(getParamString(request, name));
};