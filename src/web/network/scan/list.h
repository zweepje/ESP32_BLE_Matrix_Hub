#pragma once
#include <WiFi.h>
#include <ArduinoJson.h>
#include "web/Endpoint.h"

Endpoint networkScanList("/network/scan/list", HTTP_GET, [](AsyncWebServerRequest* request) {
    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_RUNNING) {
        JsonDocument doc; 
        doc["status"] = "scanning";
        String json;
        serializeJson(doc, json);
        request->send(200, "application/json", json);
        return;
    }

    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();
    for (int i = 0; i < n; ++i) {
        JsonObject obj = arr.createNestedObject();
        obj["ssid"] = WiFi.SSID(i);
        obj["rssi"] = WiFi.RSSI(i);
        obj["bssid"] = WiFi.BSSIDstr(i);
        obj["channel"] = WiFi.channel(i);
        obj["secure"] = WiFi.encryptionType(i);
    }

    String json;
    serializeJson(arr, json);
     request->send(200, "application/json", json);
});