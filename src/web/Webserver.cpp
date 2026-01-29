#include "Webserver.h"
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include "main.h"
#include <utils/webserial.h>

#include <cstring>

#include <string>
#include <ArduinoJson.h> // Nodig voor het parsen van de JSON commando's
#include <cstdint>
#include <map>

#include "ChunkProcessor.h"
#include "../functions/MatrixContext.h"

#include <Preferences.h>

Preferences prefs;

const char wekker_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="nl">
<head>
  <meta charset="UTF-8">
  <title>Wekker instellen</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">

  <style>
    body {
      font-family: Arial, sans-serif;
      background: #111;
      color: #fff;
      margin: 0;
      padding: 20px;
    }

    h1 {
      text-align: center;
    }

    .card {
      max-width: 320px;
      margin: auto;
      padding: 20px;
      background: #222;
      border-radius: 8px;
    }

    label {
      display: block;
      margin-top: 15px;
      font-size: 14px;
    }

    input[type="number"] {
      width: 100%;
      padding: 8px;
      font-size: 18px;
      margin-top: 5px;
      border-radius: 4px;
      border: none;
    }

    .row {
      display: flex;
      gap: 10px;
    }

    .row input {
      flex: 1;
    }

    input[type="checkbox"] {
      transform: scale(1.3);
      margin-right: 8px;
    }

    button {
      width: 100%;
      margin-top: 20px;
      padding: 12px;
      font-size: 16px;
      background: #0a84ff;
      color: white;
      border: none;
      border-radius: 6px;
    }

    button:active {
      background: #0066cc;
    }
  </style>
</head>

<body>

  <h1>Wekker</h1>

  <div class="card">
    <form action="/setAlarm" method="GET">

      <label>Tijd</label>
      <div class="row">
        <input type="number" name="hour" min="0" max="23" required placeholder="Uur">
        <input type="number" name="minute" min="0" max="59" required placeholder="Min">
      </div>

      <label>
        <input type="checkbox" name="repeat" value="1">
        Elke dag herhalen
      </label>

      <button type="submit">Opslaan</button>
    </form>
  </div>

</body>
</html>
)rawliteral";



const char config_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Matrix Config</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: sans-serif; background: #f4f4f4; padding: 20px; }
    .card { background: white; max-width: 400px; margin: auto; padding: 20px; border-radius: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
    input, select { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; box-sizing: border-box; }
    .row { display: flex; align-items: center; justify-content: space-between; }
    input[type="submit"] { background: #007bff; color: white; border: none; cursor: pointer; font-weight: bold; }
  </style>
</head><body>
  <div class="card">
    <h2>Matrix Instellingen</h2>
    <form action="/save" method="POST">
      <label>Naam Display:</label>
      <input type="text" name="name" placeholder="bijv. Woonkamer">

      <label>MAC Adres:</label>
      <input type="text" name="mac" placeholder="AA:BB:CC:DD:EE:FF">

      <label>Functie:</label>
      <select name="mode">
        <option value="kookwekker">KookWekker</option>
        <option value="temp">Temperatuur</option>
        <option value="info">Informatie</option>
		<option value="wekker">Wekker</option>
      </select>
      <label>Type Matrix:</label>
      <select name="type">
        <option value="64x16">64 x 16 (Breed)</option>
        <option value="32x32">32 x 32 (Vierkant)</option>
      </select>

      <div class="row">
        <label>Matrix Actief:</label>
        <input type="checkbox" name="active" value="1" style="width: 20px;" checked>
      </div>

      <input type="submit" value="Instellingen Opslaan">
    </form>
  </div>
</body></html>)rawliteral";



String getDynamicHTML() {
  prefs.begin("config", true);

  String html = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Matrix Control Center</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: sans-serif; background: #eceff1; padding: 20px; color: #333; }
    .container { max-width: 600px; margin: auto; background: white; padding: 25px; border-radius: 15px; box-shadow: 0 10px 25px rgba(0,0,0,0.1); }
    h2 { color: #007bff; border-bottom: 2px solid #eee; padding-bottom: 10px; }
    .matrix-card { border: 1px solid #e0e0e0; padding: 15px; margin-bottom: 20px; border-radius: 8px; background: #fafafa; }
    label { font-weight: bold; font-size: 0.9em; display: block; margin-top: 10px; }
    input, select { width: 100%; padding: 10px; margin-top: 5px; border: 1px solid #ddd; border-radius: 5px; box-sizing: border-box; }
    .active-row { display: flex; align-items: center; margin-top: 10px; background: #e3f2fd; padding: 8px; border-radius: 5px; }
    .active-row input { width: auto; margin-right: 10px; margin-top: 0; }
    .btn { background: #28a745; color: white; border: none; padding: 15px; width: 100%; border-radius: 8px; font-size: 1.1em; cursor: pointer; margin-top: 10px; }
    .btn:hover { background: #218838; }
  </style>
</head><body>
  <div class="container">
    <h2>Matrix Configuraties</h2>
    <form action="/save" method="POST">)rawliteral";

  for (int i = 0; i < 4; i++) {
    // Haal huidige waarden op uit geheugen
    String m_mac  = prefs.getString(("mac_" + String(i)).c_str(), "");
    String m_name = prefs.getString(("name_" + String(i)).c_str(), "Matrix " + String(i+1));
    String m_mode = prefs.getString(("mode_" + String(i)).c_str(), "tijd");
    String m_type = prefs.getString(("type_" + String(i)).c_str(), "64x16");
    bool m_active = prefs.getBool(("act_" + String(i)).c_str(), false);

    html += "<div class='matrix-card'>";
    html += "<h3>Matrix " + String(i+1) + "</h3>";

    html += "<label>Naam:</label>";
    html += "<input type='text' name='name_" + String(i) + "' value='" + m_name + "'>";

    html += "<label>MAC Adres:</label>";
    html += "<input type='text' name='mac_" + String(i) + "' value='" + m_mac + "' placeholder='AA:BB:CC:DD:EE:FF'>";

    html += "<label>Functie:</label>";
    html += "<select name='mode_" + String(i) + "'>";
    html += "<option value='wekker'" + String(m_mode == "wekker" ? " selected" : "") + ">Wekker / Tijd</option>";
    html += "<option value='temp'" + String(m_mode == "temp" ? " selected" : "") + ">Temperatuur</option>";
    html += "<option value='info'" + String(m_mode == "info" ? " selected" : "") + ">Informatie / Tekst</option>";
    html += "<option value='kookwekker'" + String(m_mode == "kookwekker" ? " selected" : "") + ">Kookwekker / Tekst</option>";
    html += "</select>";

    html += "<label>Type:</label>";
    html += "<select name='type_" + String(i) + "'>";
    html += "<option value='64x16'" + String(m_type == "64x16" ? " selected" : "") + ">64 x 16</option>";
    html += "<option value='32x32'" + String(m_type == "32x32" ? " selected" : "") + ">32 x 32</option>";
    html += "</select>";

    html += "<div class='active-row'>";
    html += "<input type='checkbox' name='act_" + String(i) + "' value='1'" + (m_active ? " checked" : "") + ">";
    html += "<span>Deze matrix gebruiken</span>";
    html += "</div>";

    html += "</div>";
  }

  html += R"rawliteral(
      <input type="submit" class="btn" value="Alle Instellingen Opslaan">
    </form>
  </div>
</body></html>)rawliteral";

  prefs.end();
  return html;
}













struct ClientState;
class iPixelDevice;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient* currentClient = nullptr;
extern std::map<std::string, iPixelDevice> matrixRegistry;
// Globale map om de status van alle verbonden PC's bij te houden
extern  std::map<uint32_t, ClientState> clientStates; // Key is client ID

int COLLECTING = 1 ;
int COMMANDS = 2 ;
int parsestate = COMMANDS ;
const char* hexsize ;
int currentindex = 0 ;
char *gifbuffer ;
ChunkProcessor *chunkProcessor;



char * socketData;
int currSocketBufferIndex = 0;
void setupBuffers() {

    int buffersize = 1 * 4096; // Bijvoorbeeld 16K
    socketData = (char *)heap_caps_malloc(buffersize, MALLOC_CAP_SPIRAM);
    if ( socketData == NULL ) {
        Serial.printf("Malloc socket buffer failed\n");
    }

    buffersize = 8 * 4096; // Bijvoorbeeld 16K
    gifbuffer = (char *)heap_caps_malloc(buffersize, MALLOC_CAP_SPIRAM);
    if ( gifbuffer == NULL ) {
        Serial.printf("Malloc gifbuffer  failed\n");
    }

}

void printdata( uint8_t *data, int len ) {

    Serial.printf( "Received %d bytes\n", len );

    for ( int i = 0; i < len; i++ ) {
        Serial.printf( "%c", data[i] );
    }
    Serial.println( "");
}


void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {


    AsyncWebServerRequest *request;

    switch (type) {

        case WS_EVT_CONNECT: {
            currentClient = client; // Sla de nieuwe client op
            String mac ;

            request = (AsyncWebServerRequest *)arg;
            if (request && request->hasParam("mac")) {
                mac = request->getParam("mac")->value();
                Serial.println("Client MAC: " + mac);
            } else {
                Serial.println("Geen mac parameter");
            	return;
            }

            if (client && client->client()) {
                client->client()->setNoDelay(true);
                Serial.println("TCP NoDelay geactiveerd via AsyncClient");
            }



            Serial.printf("WebSocket Client #%u verbonden.\n", client->id());
            // Voeg nieuwe client toe aan de statusmap
            clientStates[client->id()] = ClientState();
            Serial.printf("Client #%u verbonden.\n", client->id());

            //
            // start the BLE connection
            //
            const char* macAddressStr = mac.c_str() ;
        	NimBLEAddress bleAddress(macAddressStr, 0);
        	// zoek het iPixelDevice
			iPixelDevice *targetDevice = finddevice( mac ) ;
        	if ( targetDevice != nullptr ) {
        		ClientState& state = clientStates.at(client->id());
        		state.assignedMatrix = targetDevice;
        		client->text("ACK: Succesvol toegewezen aan MAC: " + String(macAddressStr));
        	} else {

        		Serial.printf("Could not find mac %s\n", macAddressStr);
        	}



            break;
        }
        case WS_EVT_DISCONNECT:
            if (currentClient == client) {
                currentClient = nullptr;
            }
            Serial.printf("WebSocket Client #%u verbroken.\n", client->id());
            // Verwijder client bij disconnect
            clientStates.erase(client->id());
            break;

        case WS_EVT_DATA: {
			debugPrintf("%s - Data ontvangen client: #%u, lengte %d\n", getLocalTimestamp().c_str(), client->id(), len);

            // Controleer of het tekstdata is (JSON)
            AwsFrameInfo *info = (AwsFrameInfo*)arg;
            if (info->opcode != WS_TEXT) {
                debugPrintf("Data was no text\n" );
                return;
            }

            //
            // copy what we got
            //

            for (size_t i = 0; i < len; i++){
                socketData[currSocketBufferIndex] = data[i];
                currSocketBufferIndex++;
            }

            if ( currSocketBufferIndex >= (info->len) ){

                    socketData[currSocketBufferIndex] = '\0';
                    Serial.printf("Data compleet: #%u, lengte %d\n", client->id(), currSocketBufferIndex );
                     len = currSocketBufferIndex ;
                     currSocketBufferIndex = 0;
            } else {
                // wait for more data
                debugPrintf("Waiting for more data\n"   );
                return ;
            }
            debugPrintf("Processing data\n"   );


            //
            // Moeten we hier al json eruit zoeken?
            //
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
            StaticJsonDocument<4096> doc; // of StaticJsonDocument<CAPACITY> doc
#pragma GCC diagnostic pop

            DeserializationError error = deserializeJson(doc, (char*)socketData, len);
            if (error) {
                debugPrintf("JSON error: %d\n", error );
                debugPrintf("Data ontvangen Ongeldig JSON-formaat\n", client->id());
                client->text("ERROR: Ongeldig JSON-formaat.");
                return;
            }


            // Zoek de status van de client
            ClientState& state = clientStates.at(client->id());
            const char* command = doc["command"];
        	state.assignedMatrix->lastNodeRedID = client->id();

#ifdef KOOKWEKKER
            // Put doc in queue for later processing
            state.assignedMatrix->enqueueCommand(doc);
            client->text("ACK: Commando in queue geplaatst.");

#else

            if ( command==nullptr ) {
                Serial.printf("command = NULL\n" );
                return ;
            }
            Serial.printf("[WEBSERVER] command = <%s>\n", command );

            if ( strcasecmp(command, "start") == 0 ||
                 strcasecmp(command, "chunk") == 0 ||
                 strcasecmp(command, "end") == 0            ) {

                if (  strcasecmp(command, "start") == 0 ) {
                    size_t size = doc["size"];
                    chunkProcessor = new ChunkProcessor( size );
                    return ;
                }
                /*
                                    parsestate = COLLECTING ;
                                    hexsize = doc["size"];
                                    Serial.printf("collecting %d bytes>\n", hexsize );
                                    currentindex = 0 ;
                                    // copy piece to gifbuffer
                                    const char *data = doc["data"];

                                    for ( int i = 0 ; i < strlen(data);  i++ ) {
                                        gifbuffer[currentindex] = data[i];
                                        currentindex++;
                                    }
                                    return ;
                                }
                                */
                if (  strcasecmp(command, "chunk") == 0 && chunkProcessor != nullptr ) {

                    debugPrintf("Processing chunk\n" );
                    const char *data = doc["data"];
                    chunkProcessor->process( data );
                    return ;
                }

                if (  strcasecmp(command, "end") == 0 && chunkProcessor != nullptr ) {

                    debugPrintf("End of chunck train\n" );
                    const char *data = doc["data"];
                    std::vector<uint8_t> *databuffer = chunkProcessor->ending();
                    if ( databuffer==nullptr ) {
                        return ;
                    }

                    //
                    // Now we can push the complete command to the queue
                    //
                    DynamicJsonDocument doc(databuffer->size() + 512);
                    doc["command"] = "send_gif";
                    JsonArray params = doc.createNestedArray("params");

                    params.add((char*)databuffer->data());

                    state.assignedMatrix->enqueueCommand(doc);
                    databuffer->clear();

                    if (state.assignedMatrix != nullptr) {
                        state.assignedMatrix->enqueueCommand(doc);
                    }
                    return ;
                }
            }



            if (state.assignedMatrix != nullptr) {
                //
                // There is a valid connection with an assigned matrix
                //
                // Normale commando verwerking (zoals SET_COLOR) gaat verder
                state.assignedMatrix->enqueueCommand(doc);
                client->text("ACK: Commando in queue geplaatst.");
                Serial.printf("Commando <%s> received\n", command);
            } else {
                Serial.printf("Commando with no matrix <%s> received\n", command);
            }
            break;
#endif
        }
    }
}

void init_webserver() {

    debugPrintf("[Webserver] Initializing webserver...\n") ;
    setupBuffers() ; // reserve socket buffer

    // Registreer de event handler bij de WebSockets server
    ws.onEvent(onWsEvent);
    // Voeg de WebSockets handler toe aan de HTTP server
    server.addHandler(&ws);
    // Start de HTTP/WebSocket serverd



  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send(200, "text/html", getDynamicHTML());
  });


  // 2. Sla alles op
  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
    prefs.begin("config", false);

  	debugPrintf("serveron\n");
    for (int i = 0; i < 4; i++) {
      if (request->hasParam("name_" + String(i), true)) {
  	debugPrintf("1serveron\n");
        prefs.putString(("name_" + String(i)).c_str(), request->getParam("name_" + String(i), true)->value());
  	debugPrintf("2serveron\n");
        prefs.putString(("mac_" + String(i)).c_str(),  request->getParam("mac_" + String(i), true)->value());
  	debugPrintf("s3erveron\n");
        prefs.putString(("mode_" + String(i)).c_str(), request->getParam("mode_" + String(i), true)->value());
  	debugPrintf("s4erveron mode %s\n", request->getParam("mode_" + String(i), true)->value().c_str()) ;
        prefs.putString(("type_" + String(i)).c_str(), request->getParam("type_" + String(i), true)->value());
    debugPrintf("5serveron\n");

        // Checkbox logica: als param niet aanwezig is, is hij uitgevinkt
        bool isActive = request->hasParam("act_" + String(i), true);
        prefs.putBool(("act_" + String(i)).c_str(), isActive);
      }
    }   prefs.end();
    request->send(200, "text/plain", "Instellingen opgeslagen! Herstarten...");
    delay(1000);
    ESP.restart();
  });


	server.on("/wekker", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(200, "text/html", wekker_html );
	});


	server.on("/setAlarm", HTTP_GET, [](AsyncWebServerRequest *request) {

	  int hour = request->getParam("hour")->value().toInt();
	  int minute = request->getParam("minute")->value().toInt();
	  bool repeat = request->hasParam("repeat");

	  Serial.printf("Alarm: %02d:%02d repeat=%d\n", hour, minute, repeat);

	  request->send(200, "text/plain", "Wekker ingesteld");
	});



/*
  // 4. De route om de data van het formulier te verwerken
  server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
  if (request->hasParam("mac")) {
    String newMac = request->getParam("mac")->value();
    Serial.println("Ontvangen MAC via URL: " + newMac);
    request->send(200, "text/plain", "Ontvangen: " + newMac);
  } else {
    request->send(200, "text/plain", "Geen mac parameter gevonden. Gebruik: /test?mac=123");
  }
});
*/


    server.begin();
    Serial.println("WebSocket Server gestart op ws://" + WiFi.localIP().toString() + "/ws");
  //  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  //	debugPrintf("[Webserver] Webserver initialized!\n") ;
}
