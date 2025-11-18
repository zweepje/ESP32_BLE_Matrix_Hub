#pragma once
#include <WiFi.h>
#include <ArduinoJson.h>
#include "web/Endpoint.h"

Endpoint networkScanStart("/network/scan/start", HTTP_GET, [](AsyncWebServerRequest* request) {
    WiFi.scanDelete();                
    WiFi.scanNetworks(true);

    JsonDocument doc;
    doc["status"] = "scanning";
                
    String response;
    serializeJson(doc, response);
    request->send(202, "application/json", response);
});