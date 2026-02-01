#include "main.h"
#include "utils/webserial.h"
#include "global.h"
#include "Constants.h"
#include <Arduino.h>
#include "utils/webserial.h"
#include <Preferences.h>
#include "Bluetooth.h"
#include "iPixelDeviceRegistry.h"
#include "web/Webserver.h"
#include <LittleFS.h>
#include <map>
#include <string>

#include "clock/timefunctions.h"
#include "functions/MatrixContext.h"
#include "version.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Display/oleddisplay.h"
#include "audio/AudioPlayer.h"
#include "i2c/I2CSetup.h"
#include "audio/I2SSetup.h"
#include "audio/WavPlayer.h"
#include "esp_wifi.h"
#include "Display/Display.h"

extern "C" {
void initfs(void);
}

bool debugbuttons = false;

Preferences preferences;


// Globale map om de status van alle verbonden PC's bij te houden
std::map<uint32_t, ClientState> clientStates; // Key is client ID
std::map<std::string, iPixelDevice> matrixRegistry;

extern AsyncWebSocket ws;

void loop_connected();
void setup_connected();
void setup_wifi_post();

Display *display;

uint8_t g_debugFlags = DEBUG_QUEUE | DEBUG_BLE | DEBUG_BLE2;
// Initialisatie

//WavPlayer player;
AudioPlayer audio;
AlarmSettings activeAlarms;
volatile bool alarmChanged = false ;


MatrixMode getMode( String mstr ) {


	debugPrintf("Mode string is %s\n", mstr.c_str());
    if (mstr.equalsIgnoreCase("kookwekker") ) return MODE_KOOKWEKKER ;
    if (mstr.equalsIgnoreCase("info") ) return MODE_INFO ;
	if (mstr.equalsIgnoreCase("temp") ) return MODE_TEMP ;
	if (mstr.equalsIgnoreCase("wekker") ) return MODE_WEKKER ;
	//delay( 50000);
    return MODE_NONE ;
}


bool hasDisplay = false;


touch_value_t touchRead(int p) {
	return 0 ;
}

#include <NimBLEDevice.h>

void scanBLEDevices() {
	Serial.println("BLE Scan gestart...");
	NimBLEScan* pBLEScan = NimBLEDevice::getScan();

	// FIX 1: start() geeft een bool, geen resultaten.
	pBLEScan->start(5, false);
	NimBLEScanResults results = pBLEScan->getResults();

	Serial.printf("Scan klaar. %d apparaten gevonden:\n", results.getCount());

	for (int i = 0; i < results.getCount(); i++) {
		// FIX 2: Gebruik NimBLEAdvertisedDevice* (pointer)
		const NimBLEAdvertisedDevice* device = results.getDevice(i);

		String name = device->getName().c_str(); // Gebruik -> ipv .
		if (name == "") name = "Onbekend";

		Serial.printf("[%d] Naam: %s | Adres: %s | RSSI: %d\n",
					  i,
					  name.c_str(),
					  device->getAddress().toString().c_str(),
					  device->getRSSI());
	}
	pBLEScan->clearResults();
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


	WiFi.mode(WIFI_STA);
	//WiFi.setSleep(false);
	WiFi.setTxPower(wifi_power_t(40));
	esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G);
	Serial.print("[WiFi] Connecting to SSID: " + ssid);

	WiFi.begin(ssid, pass );

	int counter = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		int s = WiFi.status();
		if (s == WL_NO_SSID_AVAIL) Serial.print("Niet gevonden ");
		if (s == WL_CONNECT_FAILED) Serial.print("Wachtwoord fout ");
		if (s == WL_CONNECTION_LOST) Serial.print("Verbinding verloren ");
		if (s == WL_DISCONNECTED) Serial.print(". ");

		counter++;
		if (counter > 20) { // Na 10 seconden opnieuw proberen
			Serial.println("\nResetten en opnieuw proberen...");
			WiFi.disconnect();

			WiFi.mode(WIFI_STA);
			//WiFi.setSleep(false);
			WiFi.setTxPower(wifi_power_t(40));
			esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G);
			Serial.println("Verbinden met network");
			WiFi.begin(ssid, pass );

			counter = 0;
		}
	}
	Serial.println("\nVerbonden!");

  Serial.println("[WiFi] Connected!");

  Serial.print("[WiFi] IP: ");
  Serial.println(WiFi.localIP());
  setup_connected();
}




//
// Deze info kan ook in iPixeldevice!!!
//
typedef struct display {
  String name ;
  String MAC ;
  String type ;
  int width, height;
  String function ;
  iPixelDevice *device;
} t_display ;


#define MAX_MATRIX 4
t_display displays[MAX_MATRIX] = {
//  { "2B:46:4E:59:CD:A0", "32x32", 32,32,"temperatuur", NULL },
//  { "C6:2A:E6:06:7D:10", "64x16", 64, 16,"info", NULL },
//  { "70:21:BC:EA:A8:24", "32x32", 32,32,"kookwekker", NULL }
};
int numdisplays = 0;


iPixelDevice *finddevice( String mac ) {

	for ( int i = 0 ; i < numdisplays ; i++ ) {
		if ( displays[i].MAC.equalsIgnoreCase(mac)  ) {
			debugPrintf("Found display\n");
			return displays[i].device ;
		}
	}
	debugPrintf("Could not find display %s\n", mac.c_str() );
	return nullptr;
}

extern Preferences prefs ;

void initdevices() {

        numdisplays = 0 ;
        prefs.begin("config", true);

        for (int i = 0 ; i < MAX_MATRIX ; i++ ) {

          bool active = prefs.getBool(("act_" + String(i)).c_str(), false);
          debugPrintf("Trying  act_%s, value is: %s\n", String(i).c_str(), active?"yes":"no");

          if ( active ) {
            displays[i].name   = prefs.getString(("name_" + String(i)).c_str(), "Matrix " + String(i+1));
            displays[i].MAC    = prefs.getString(("mac_" + String(i)).c_str(), "");
            displays[i].function   = prefs.getString(("mode_" + String(i)).c_str(), "default");
            displays[i].type   = prefs.getString(("type_" + String(i)).c_str(), "64x16");
            displays[i].width  = 32 ;
            displays[i].height = 32 ;


            const char* macAddressStr = displays[i].MAC.c_str() ;
            NimBLEAddress bleAddress(macAddressStr, 0);
            displays[i].device = new iPixelDevice(bleAddress);
            auto* context = new (std::nothrow) MatrixContext(displays[i].width, displays[i].height, 8);
            displays[i].device->context_data = static_cast<void*>(context);
            numdisplays++;

            // tijd/temp/info

            iPixelDevice *dev = displays[i].device;

            dev->mode = getMode( displays[i].function );

            debugPrintf("Added %s to the active list mode: %d\n", macAddressStr, dev->mode );
          }
        }
        prefs.end();
}

// De taak die op de achtergrond draait
void connectionTask(void * pvParameters) {


	for(;;) { // Oneindige loop voor de task
		for (int i = 0; i < numdisplays; i++) {

			//debugPrintf("Testing display %d, %s for %s\n", i, displays[i].MAC.c_str(), displays[i].function.c_str());
			debugPrintf("Matrix: %s is %s %s\n", displays[i].function.c_str(), ( displays[i].device->connected ? "connected" : "not connected" ),
			( displays[i].device->connecting ? "and connecting" : "" ) ) ;


			//if ( !displays[i].device->connected && !displays[i].device->connecting ) {
			if ( !displays[i].device->connected  ) {
				//
				if ( displays[i].device->connecting ) {
					// It had 10 seconds to connect, so it fails.
					// retry
					if ( displays[i].device->client != nullptr ) {
					displays[i].device->client->disconnect();
					}
				}
				// start a connection
				displays[i].device->connectAsync();
				Serial.printf("Queued BLE connectie met %s\n", displays[i].type.c_str());
				vTaskDelay(pdMS_TO_TICKS(2000)); // wacht 2 seconden
			}



      //if (!isDisplayConnected(i)) {
      //  Serial.printf("[Task] Display %d is offline. Scannen...\n", i);

        // Hier voer je de scan/connect logica uit
        // Deze pauzeert de main loop NIET!
       // attemptConnection(knownAddresses[i]);
      //}
    }


    // Wacht 10 seconden voor de volgende check (vTaskDelay gebruikt geen CPU)
    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}


void setup() {

  Serial.begin(115200);
  delay(200); // Wacht even op de seriële monitor
	Serial.println("ESP - BLE matrix HUB");
	Serial.println("================================");
	Serial.printf("Software Versie: %s\n", VERSION);
	Serial.printf("Build Nummer:    %s\n", BUILD_NUMBER);
	Serial.printf("Git Branch:      %s\n", GIT_BRANCH);
	Serial.printf("Board:           %s\n", BOARD);
	Serial.printf("Build Datum:     %s\n", BUILD_DATE);
	Serial.println("================================");

  Serial.println("\n--- PSRAM/Geheugen Status ---");
  // 1. Controleer de totale beschikbare DRAM (interne heap)
  size_t dram_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  Serial.printf("Interne DRAM Heap: %u bytes\n", dram_heap);

  // 2. Controleer de totale beschikbare PSRAM (externe heap)
  size_t psram_size = heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  Serial.printf("Externe PSRAM Heap: %u bytes\n", psram_size);


	display = new Display();    // can only start after i2c assigned!

	if (display->isAvailable() ) {
		Serial.print("Display available\n");
	} else {
		Serial.print("Display not detected\n");
	}
	display->SetStatus("IP config");
	setup_wifi_post();
	display->SetIp(WiFi.localIP().toString());
	display->SetStatus("Wifi configured");



  initTime();
  //
  // Maak de achtergrond-taak aan
  //

  xTaskCreatePinnedToCore(
      connectionTask,   // Functie die uitgevoerd moet worden
      "ConnManager",    // Naam van de taak
      4096,             // Stack size in bytes
      NULL,             // Parameter die je meegeeft
      1,                // Prioriteit (1 is laag, prima voor dit doel)
      NULL,             // Task handle
      0                 // Pin de taak aan Core 0 (Wi-Fi/BT core)
  );



	if (!LittleFS.begin()) {
		Serial.println("LittleFS mount failed");
		return;
	}

	setupI2C(8, 9);   // ✅ alleen I2C
	setupI2S();       // ✅ alleen audio
	Serial.println("Start playing\n");

	WavPlayer player;
//	wisScherm();
	//schrijfTekst( "start play", 10,10,2 );
	//player.play("/beep.wav");
	audio.startPlay("/alarm.wav");
	Serial.println("Playing done\n");
//	wisScherm();
//	schrijfTekst( "done", 10,10,2 );
	delay(1000);


}



const int TOUCH_START_STOP = 1; // De pin die mooi bij de rest zit
const int TOUCH_SECONDS = 13; // De pin die mooi bij de rest zit
const int TOUCH_MINUTES = 12; // De pin die mooi bij de rest zit
TouchButton btnMinutes(TOUCH_MINUTES) ;
TouchButton btnSeconds(TOUCH_SECONDS);
TouchButton btnStart(TOUCH_START_STOP);

unsigned long previousMillis = 0 ;
unsigned long interval = 1000 ;  // each second
unsigned int cnt = 0 ;


void updateAlarmCache() {
	preferences.begin("alarm-clock", true); // true = read-only mode

	activeAlarms.minOnce = preferences.getInt("min_once", -1);
	activeAlarms.enOnce  = preferences.getBool("en_once", false);

	activeAlarms.minWork = preferences.getInt("min_work", -1);
	activeAlarms.enWork  = preferences.getBool("en_work", false);

	activeAlarms.minWeekend = preferences.getInt("min_weekend", -1);
	activeAlarms.enWeekend  = preferences.getBool("en_weekend", false);

	preferences.end();
	Serial.println("RAM-cache bijgewerkt met nieuwe alarmtijden.");
}












void loop() {

	if (alarmChanged) {
		alarmChanged = false;
		updateAlarmCache();
	}



  unsigned long currentMillis = millis();
  // Controleer of er 60 seconden zijn verstreken sinds de laatste afdruk
  if (currentMillis - previousMillis >= interval) {
	  previousMillis = currentMillis;
  	String time = getCurrentTimeString();
	cnt++ ;
  	if ( cnt>=10 ) {
  		cnt = 0 ;
  		debugPrintf( "======== Time is: %s ========\n",time.c_str() );
  		//wisScherm();
  		//schrijfTekst( time.c_str(), 10, 10, 2 ) ;
  	}
  	display->SetTime( time.c_str() );
  }
  loop_connected();

}

void loop_connected() {

    ws.cleanupClients();
	delay(500);
	for (int i = 0; i < numdisplays; i++) {

	    iPixelDevice *dev = displays[i].device;

	    if ( dev != nullptr ) {

	    	debugPrintf("mode is %d\n", dev->mode );

 			if ( dev->mode == MODE_KOOKWEKKER  ) {
				// als het wekker is:
				dev->handleTimerLogic();
 				if ( debugbuttons ) delay( 500 );
			} else
			if (dev->mode == MODE_WEKKER) {

					dev->handleWekker() ;
			}
			dev->update();
			dev->queueTick();
	    }
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

	initdevices() ;

	init_bluetooth();
	init_webserver();
	WebSerial.setBuffer( 0 );
	WebSerial.begin(&server);
	//delay(2000) ;

	// Rapportage zodra WebSerial klaar is
	debugPrintf("--- ESP32-S3 BOOT REPORT ---\n");
	debugPrintf("[SYS] Reset Reason: %s\n", getResetReason().c_str());
	debugPrintf("[SYS] Free Heap: %u bytes\n", ESP.getFreeHeap());
	debugPrintf("[WIFI] Connected! IP: %s (RSSI: %d)\n", WiFi.localIP().toString().c_str(), WiFi.RSSI());

	// De MTU die we op 14 dec wilden checken:
	debugPrintf("[BLE] Auto-MTU Detection: Ready\n");
	debugPrintf("----------------------------\n");

}
