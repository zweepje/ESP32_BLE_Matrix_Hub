#include "main.h"
#include <Arduino.h>
#include <ImprovWiFiLibrary.h>
#include <Preferences.h>
#include "Bluetooth.h"
#include "iPixelDeviceRegistry.h"
#include "web/Webserver.h"
#include <LittleFS.h>
#include <map>
#include <string>

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

void onImprovWiFiErrorCb(ImprovTypes::Error err) {
  Serial.println("[Improv] WiFi failed! Reconnecting to saved one...");
  setup_wifi_post();
}

void onImprovWiFiConnectedCb(const char *ssid, const char *password) {
  Serial.println("[Improv] Got WiFi credentials! (no, we won't leak them here :/)");
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
  Serial.println("[WiFi] Mode is now 'STATION'!");

  WiFi.disconnect();
  Serial.println("[WiFi] Disconnected after startup!");
}

void setup_wifi_post() {
  preferences.begin("wifi", false);
  preferences.putString( "ssid", "Palamedes_ExtraWiFi2G");
  preferences.putString( "pass", "Poetiniseenlul");
  preferences.end() ;

  preferences.begin("wifi", true); // read-only
  String ssid = preferences.getString("ssid", "");
  String pass = preferences.getString("pass", "");
  preferences.end();
  if(ssid.equals("")) {
    Serial.println("[WiFi] No credentials set! Not connecting!");
    return;
  }

  Serial.print("[WiFi] Connecting to SSID: " + ssid);
  WiFi.begin(ssid, pass);
  while(!WiFi.isConnected()) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("OK");
  Serial.println("[WiFi] Connected!");

  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());
  setup_connected();
}

void setup_improv() {
  Serial.println("[Improv] Setting up...");
  improvSerial.setDeviceInfo(ImprovTypes::ChipFamily::CF_ESP32, "iPixel-Server", "1.0.0", "ESP32", "http://{LOCAL_IPV4}?name=Guest");
  improvSerial.onImprovError(onImprovWiFiErrorCb);
  improvSerial.onImprovConnected(onImprovWiFiConnectedCb);
  improvSerial.setCustomConnectWiFi(connectWifi);
  Serial.println("[Improv] Ready!");
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  Serial.println("[Setup] Hello World! Let's hope we can pixel together!");

  // Probeer LittleFS te mounten
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount mislukt, proberen te formatteren...");

    // Dwing de formatteer-actie af als het mounten faalt
    if (LittleFS.format()) {
      Serial.println("LittleFS succesvol geformatteerd. Opnieuw opstarten...");
      // Herstart de chip om te proberen de schone partitie te mounten
      ESP.restart();
    } else {
      Serial.println("FATALE FOUT: Formatteren mislukt.");
    }
  } else {
    Serial.println("LittleFS succesvol gemount.");
    // Normaal programma gaat hier verder...
  }


  setup_wifi_pre();
  setup_improv();
  setup_wifi_post();


}

void loop() {
  improvSerial.handleSerial();
  if (improvSerial.isConnected()) loop_connected();
}

void loop_connected() {

    // 1. WebSocket onderhoud
    // Nodig om client time-outs af te handelen
    ws.cleanupClients();

    // 2. Queue Verwerking
    // Loop door alle geregistreerde Matrix Controllers
    for (auto& pair : matrixRegistry) {
      // pair.second is een referentie naar het iPixelDevice object
      pair.second.processQueue();
      pair.second.queueTick();
    }

    // 3. Andere taken
    // Bijv. delay(1); of yield();


  //loop_deviceregistry();
}

//  iPixelDevice test(BLEAddress("3d:50:0c:1f:6d:ec"));
//2F:9F:9C:9C:51:AC

//
// voorlopig mijn experimenten
//
void verder() {
  Serial.println("In verder....");
  //  connect met de matrix:
 // std::string mac_str("C6:2A:E6:06:7D:10");
 // NimBLEAddress  addr(mac_str, 0);

 // iPixelDevice dev = iPixelDevice( addr );
 // dev.connectAsync() ;
 // Serial.println("Klaar met connectie");
 /* dev.setBrightness(1);
  dev.queueTick();
  Serial.println("Brightness gezet");
  Serial.println("delay");
  delay(2000);
  dev.setBrightness(90);
  dev.queueTick();
  Serial.println("Brightness gezet");
  */
/*
  String text_str = "Jezus is een vriend van mij.";
  dev.sendText( text_str, 1, 1, 20, 0,0,99, 0, 16 );
  //t(const String& text,  int animation, int save_slot, int speed, uint8_t colorR, uint8_t colorG, uint8_t colorB, int rainbow_mode, int matrix_height) {
  dev.queueTick();
  dev.queueTick();
  dev.queueTick();
  */


}




void setup_connected() {
  init_bluetooth();
  init_webserver();

  verder() ;
}
