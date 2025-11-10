#include "Webserver.h"

#include "web/device/raw/clear.h"
#include "web/device/raw/deleteScreen.h"
#include "web/device/raw/sendGIF.h"
#include "web/device/raw/sendPNG.h"
#include "web/device/raw/sendText.h"
#include "web/device/raw/setBrightness.h"
#include "web/device/raw/setClockMode.h"
#include "web/device/raw/setFunMode.h"
#include "web/device/raw/setLED.h"
#include "web/device/raw/setOrientation.h"
#include "web/device/raw/setPixel.h"
#include "web/device/raw/setRhythmAnimationMode.h"
#include "web/device/raw/setRhythmLevelMode.h"
#include "web/device/raw/setSpeed.h"
#include "web/device/raw/setTime.h"

#include "web/network/scan/start.h"
#include "web/network/scan/list.h"
#include "web/network/pair/delete.h"
#include "web/network/pair/list.h"
#include "web/network/pair/upsert.h"

AsyncWebServer server(80);

void init_webserver() {
    Serial.println("[Webserver] Initializing webserver...");
    for (auto& ep : Endpoint::getRegistry()) {
        server.on(ep->path, ep->method, [handler = ep->handler](AsyncWebServerRequest* request) {
            try {
                handler(request);
            } catch (const std::invalid_argument& ex) {
                request->send(400, "text/plain", ex.what());
            } catch (const std::out_of_range& ex) {
                request->send(404, "text/plain", ex.what());
            } catch (const std::runtime_error& ex) {
                request->send(500, "text/plain", ex.what());
            } catch (const std::exception& ex) {
                request->send(500, "text/plain", ex.what());
            } catch (...) {
                request->send(500, "text/plain", "Unknown error");
            }
        });
        Serial.print("[Webserver] Registered endpoint: ");
        Serial.println(ep->path);
    }
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    server.begin();
    Serial.println("[Webserver] Webserver initialized!");
}