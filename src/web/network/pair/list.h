#pragma once
#include <WiFi.h>
#include <ArduinoJson.h>
#include "registry.h"
#include "web/Endpoint.h"

Endpoint networkPairList("/network/pair/list", HTTP_GET, [](AsyncWebServerRequest* request) {
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    for (const auto& pair : networkPairs) {
        JsonObject obj = arr.createNestedObject();
        obj["name"] = pair.name;
        obj["key"] = pair.key;
        obj["priority"] = pair.priority;
    }
    
    String json;
    serializeJson(arr, json);
    request->send(200, "application/json", json);
});