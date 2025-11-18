#pragma once
#include <WiFi.h>
#include <ArduinoJson.h>
#include "registry.h"
#include "web/Endpoint.h"

Endpoint networkPairUpsert("/network/pair/upsert", HTTP_GET, [](AsyncWebServerRequest* request) {
    if(!request->hasParam("name")) return request->send(400, "text/plain", "Missing 'name' parameter");
    if(!request->hasParam("key")) return request->send(400, "text/plain", "Missing 'key' parameter");
    if(!request->hasParam("priority")) return request->send(400, "text/plain", "Missing 'priority' parameter");

    String name = request->getParam("name")->value();
    String key = request->getParam("key")->value();
    int priority = request->getParam("priority")->value().toInt();

    for (auto& pair : networkPairs) {
        if (pair.name == name) {
            pair.key = key;
            pair.priority = priority;
            request->send(204, "text/plain", "Updated");
            return;
        }
    }
 
    networkPairs.push_back({ name, key, priority });
    request->send(201, "text/plain", "Created");
});