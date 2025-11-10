#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <vector>

class Endpoint {
    public:
        const char* path;
        WebRequestMethod method;
        ArRequestHandlerFunction handler;

        Endpoint(const char* path, WebRequestMethod method, ArRequestHandlerFunction handler): path(path), method(method), handler(handler) {
            auto& endpoints = getRegistry();
            for (auto& e : endpoints) {
                if (strcmp(e->path, path) == 0) return;
            }
            endpoints.push_back(this);
        }

        static std::vector<Endpoint*>& getRegistry() {
            static std::vector<Endpoint*> registry;
            return registry;
        }
};