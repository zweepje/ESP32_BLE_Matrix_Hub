#include "main.h"
#include "global.h"
#include <Arduino.h>
#include "utils/webserial.h"
#include <ImprovWiFiLibrary.h>
#include <Preferences.h>
#include "Bluetooth.h"
#include "iPixelDeviceRegistry.h"
#include "web/Webserver.h"
#include <LittleFS.h>
#include <map>
#include <string>
#include "clock/timefunctions.h"

extern "C" {
void initfs(void);
}

Preferences preferences;
ImprovWiFi improvSerial(&Serial);

#define MATRIX1_64x16 "C6:2A:E6:06:7D:10"
#define MATRIX2_32x32 "2B:46:4E:59:CD:A0"

// Globale map om de status van alle verbonden PC's bij te houden
std::map<uint32_t, ClientState> clientStates; // Key is client ID
std::map<std::string, iPixelDevice> matrixRegistry;

extern AsyncWebSocket ws;

void loop_connected();
void setup_connected();
void setup_wifi_post();


uint8_t g_debugFlags = DEBUG_QUEUE | DEBUG_BLE | DEBUG_BLE2;
// Initialisatie






void onImprovWiFiErrorCb(ImprovTypes::Error err) {
  Serial.println("[Improv] WiFi failed! Reconnecting to saved one...");
  setup_wifi_post();
}

void onImprovWiFiConnectedCb(const char *ssid, const char *password) {
  DBG_PRINTF( DEBUG_WIFI,"[Improv] Got WiFi credentials! (no, we won't leak them here :/)");
  preferences.begin("wifi", false); // namespace "wifi"
  preferences.putString("ssid", ssid);
  preferences.putString("pass", password);
  preferences.end();
  setup_wifi_post();
}

bool connectWifi(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  while (!improvSerial.isConnected()) {}
  return true;
}

void setup_wifi_pre() {
  WiFi.mode(WIFI_STA);
  DBG_PRINTF( DEBUG_WIFI,"[WiFi] Mode is now 'STATION'!");

  delay(500);

  WiFi.disconnect();
  DBG_PRINTF( DEBUG_WIFI,"[WiFi] Disconnected after startup!");
}

void setup_wifi_post() {
  //
  /*
  preferences.begin("wifi", false);
  preferences.putString( "ssid", "Palamedes_ExtraWiFi2G");
  preferences.putString( "pass", "Poetiniseenlul");
  preferences.end() ;

  preferences.begin("wifi", true); // read-only
  String ssid = preferences.getString("ssid", "");
  String pass = preferences.getString("pass", "");
  preferences.end();
  if(ssid.equals("")) {
    DBG_PRINTF( DEBUG_WIFI,"[WiFi] No credentials set! Not connecting!");
    return;
  }
*/
  String ssid = "Palamedes_ExtraWiFi2G" ;
  String pass = "Poetiniseenlul" ;

  Serial.print("[WiFi] Connecting to SSID: " + ssid);
  WiFi.begin(ssid, pass);
  Serial.print("aap");
  delay(5000);
  Serial.print("noot");

  while(!WiFi.isConnected()) {
    Serial.print(".");
    delay(500);
  }
  delay(2000); // wacht 2 seconden
  Serial.println("OK");
  Serial.println("[WiFi] Connected!");

  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());
  setup_connected();
}

void setup_improv() {
/*
  Serial.println("[Improv] Setting up...");
  improvSerial.setDeviceInfo(ImprovTypes::ChipFamily::CF_ESP32, "iPixel-Server", "1.0.0", "ESP32", "http://{LOCAL_IPV4}?name=Guest");
  improvSerial.onImprovError(onImprovWiFiErrorCb);
  improvSerial.onImprovConnected(onImprovWiFiConnectedCb);
  improvSerial.setCustomConnectWiFi(connectWifi);
  Serial.println("[Improv] Ready!");
*/
}


void setup() {

  Serial.begin(115200);
  delay(500); // Wacht even op de seriële monitor
  Serial.println("[Setup] Hello World! Let's hope we can pixel together!");
  delay(500); // for debug


  Serial.println("\n--- PSRAM/Geheugen Status ---");
  // 1. Controleer de totale beschikbare DRAM (interne heap)
  size_t dram_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  Serial.printf("Interne DRAM Heap: %u bytes\n", dram_heap);

  // 2. Controleer de totale beschikbare PSRAM (externe heap)
  size_t psram_size = heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  Serial.printf("Externe PSRAM Heap: %u bytes\n", psram_size);

  // -----------------------------------------------------

  if (psram_size > 0) {
    // 3. PSRAM is gedetecteerd, probeer een grote buffer toe te wijzen
    const size_t TEST_SIZE = 1024 * 512; // 512 KB, dit past NIET in interne DRAM

    Serial.printf("\nProbeer %u KB toe te wijzen in PSRAM...", TEST_SIZE / 1024);

    // Gebruik MALLOC_CAP_SPIRAM om te garanderen dat het geheugen van de PSRAM komt
    uint8_t *large_buffer = (uint8_t *)heap_caps_malloc(TEST_SIZE, MALLOC_CAP_SPIRAM);

    if (large_buffer != NULL) {
      // Allocatie gelukt! Schrijf wat data om te controleren
      memset(large_buffer, 0xAA, TEST_SIZE);
      Serial.println("✅ Succesvol toegewezen en getest! PSRAM werkt.");

      // Belangrijk: Geef geheugen vrij
      heap_caps_free(large_buffer);
    } else {
      Serial.println("❌ Fout: Allocatie mislukt, ondanks gemelde vrije PSRAM.");
    }
  } else {
    Serial.println("❌ PSRAM is niet gedetecteerd (gemelde grootte is 0).");
  }
  Serial.println("-------------------------------------");

  setup_wifi_pre();
  setup_improv();
  setup_wifi_post();

  initTime();
}


unsigned long previousMillis = 0 ;
unsigned long interval = 60000 ;

void loop() {

  unsigned long currentMillis = millis();
  // Controleer of er 60 seconden zijn verstreken sinds de laatste afdruk
  if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      String time = getCurrentTimeString();

      debugPrintf( "======== Time is: %s ========\n",time.c_str() );

  }

  loop_connected();

}

void loop_connected() {

    // 1. WebSocket onderhoud
    // Nodig om client time-outs af te handelen
    ws.cleanupClients();

    // 2. Queue Verwerking
    // Loop door alle geregistreerde Matrix Controllers
    for (auto& pair : matrixRegistry) {
      // pair.second is een referentie naar het iPixelDevice object
      //pair.second.processQueue();
      pair.second.update();
      pair.second.queueTick();
    }

}

//  iPixelDevice test(BLEAddress("3d:50:0c:1f:6d:ec"));
//2F:9F:9C:9C:51:AC



String getResetReason() {
  esp_reset_reason_t reason = esp_reset_reason();
  switch (reason) {
    case ESP_RST_POWERON: return "Power On / Hard Reset";
    case ESP_RST_SW:      return "Software Restart";
    case ESP_RST_PANIC:   return "System Panic (Crash)";
    case ESP_RST_INT_WDT: return "Interrupt Watchdog (Hung)";
    case ESP_RST_TASK_WDT:return "Task Watchdog (Hung)";
    case ESP_RST_BROWNOUT:return "Brownout (Voltage Drop)";
    default:              return "Unknown Reset Reason";
  }
}



void setup_connected() {
  init_bluetooth();
  init_webserver();
  WebSerial.setBuffer( 0 );
  WebSerial.begin(&server);
  delay(2000) ;

  // Rapportage zodra WebSerial klaar is
  debugPrintf("--- ESP32-S3 BOOT REPORT ---\n");
  debugPrintf("[SYS] Reset Reason: %s\n", getResetReason().c_str());
  debugPrintf("[SYS] Free Heap: %u bytes\n", ESP.getFreeHeap());
  debugPrintf("[WIFI] Connected! IP: %s (RSSI: %d)\n", WiFi.localIP().toString().c_str(), WiFi.RSSI());

  // De MTU die we op 14 dec wilden checken:
  debugPrintf("[BLE] Auto-MTU Detection: Ready\n");
  debugPrintf("----------------------------\n");

}
