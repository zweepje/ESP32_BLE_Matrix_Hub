#pragma once
#include <WiFi.h>
#include <ArduinoJson.h>
#include "registry.h"
#include "web/Endpoint.h"

Endpoint networkPairDelete("/network/pair/delete", HTTP_GET, [](AsyncWebServerRequest* request) {
    if(!request->hasParam("name")) return request->send(400, "text/plain", "Missing 'name' parameter");

    String name = request->getParam("name")->value();
    for (auto it = networkPairs.begin(); it != networkPairs.end(); ++it) {
        if (it->name == name) {
            networkPairs.erase(it);
            request->send(200, "text/plain", "Deleted");
            return;
        }
    }

    request->send(404, "text/plain", "Not found");
    return;
});