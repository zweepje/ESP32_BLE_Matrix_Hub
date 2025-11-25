#include "Webserver.h"
#include "main.h"

#include <cstring>

#include <string>
#include <ArduinoJson.h> // Nodig voor het parsen van de JSON commando's
#include <cstdint>
#include <map>


struct ClientState;
class iPixelDevice;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient* currentClient = nullptr;
extern std::map<std::string, iPixelDevice> matrixRegistry;
// Globale map om de status van alle verbonden PC's bij te houden
extern  std::map<uint32_t, ClientState> clientStates; // Key is client ID



char * socketData;
int currSocketBufferIndex = 0;
void setupBuffer() {
    // allocate memory for socketData in PSRAM if you have.
    socketData  = (char *) malloc (4096);
    if ( socketData == NULL ) {
        Serial.printf("Malloc socket buffer failed\n");
    }
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {

    switch (type) {
        case WS_EVT_CONNECT:
            currentClient = client; // Sla de nieuwe client op
            Serial.printf("WebSocket Client #%u verbonden.\n", client->id());
            // Voeg nieuwe client toe aan de statusmap
            clientStates[client->id()] = ClientState();
            Serial.printf("Client #%u verbonden.\n", client->id());
            break;

        case WS_EVT_DISCONNECT:
            if (currentClient == client) {
                currentClient = nullptr;
            }
            Serial.printf("WebSocket Client #%u verbroken.\n", client->id());
            // Verwijder client bij disconnect
            clientStates.erase(client->id());
            break;

        case WS_EVT_DATA: {

            Serial.printf("Data ontvangen client: #%u, lengte %d\n", client->id(), len );
            // Controleer of het tekstdata is (JSON)
            AwsFrameInfo *info = (AwsFrameInfo*)arg;
            if (info->opcode != WS_TEXT) return;
            //
            // copy what we got
            for (size_t i = 0; i < len; i++){
                socketData[currSocketBufferIndex] = data[i];
                currSocketBufferIndex++;
            }
            if( currSocketBufferIndex >= info->len ){
                    socketData[currSocketBufferIndex] = '\0';
                    Serial.printf("Data compleet: #%u, lengte %d\n", client->id(), currSocketBufferIndex );

                     len = currSocketBufferIndex ;
                     currSocketBufferIndex = 0;
                    // do the normal stuff
            } else {
                // wait for more data
                return ;
            }

            //for ( int i=0 ; i<bufzize)



#pragma GCC diagnostic push
            // 2. Schakel de specifieke waarschuwing uit voor de volgende regels
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
            // 3. De regel die de waarschuwing veroorzaakt:
            // JsonDocument heeft geen vooraf reservatie oid.

            // dynamic maken!!!
            StaticJsonDocument<4096> doc; // of StaticJsonDocument<CAPACITY> doc
            //SpiRamJsonDocument doc(SOCKET_DATA_SIZE);
            //JsonDocument doc ;
            // 4. Herstel de oorspronkelijke waarschuwingsstatus
#pragma GCC diagnostic pop

            DeserializationError error = deserializeJson(doc, (char*)socketData, len);



            if (error) {
                Serial.printf("JSON error: %d\n", error );
                Serial.printf("Data ontvangen Ongeldig JSON-formaat\n", client->id());
                client->text("ERROR: Ongeldig JSON-formaat.");
                return;
            }
            Serial.printf("Data was valid for client #%u\n", client->id());

            //
            // check if



            //const char* targetKey = doc["target"]; // Bijv. "MATRIX_1"
            //const char* command = doc["command"]; // Bijv. "SET_COLOR"

            const char* macAddressStr = doc["target"];

            // Zoek de status van de client
            ClientState& state = clientStates.at(client->id());
            const char* command = doc["command"];

            if (strcmp(command, "ASSIGN") == 0) {

                // 1. CONTROLEER OF HET APPARAAT REEDS BESTAAT
                if (matrixRegistry.count(macAddressStr) == 0) {

                    // --- 2. APPARAAT BESTAAT NIET: CREËER & REGISTREER DYNAMISCH ---

                    // a. Converteer de string MAC-adres naar een NimBLEAddress object
                    NimBLEAddress bleAddress(macAddressStr, 0);
              //      NimBLEAddress  addr(macAddressStr, 0);
                    // b. Creëer de iPixelDevice instantie en voeg toe aan de map
                    // De std::map voert de copy/move constructor uit

                    auto result = matrixRegistry.emplace(
                        macAddressStr,
                        iPixelDevice(bleAddress)
                        //iPixelDevice(addr)
                    );

                    Serial.printf("Nieuw iPixelDevice aangemaakt voor MAC: %s\n", macAddressStr);

                    iPixelDevice& targetDevice = result.first->second;
                    targetDevice.enqueueCommand(doc);

                    // c. Start direct de BLE verbinding voor dit nieuwe apparaat
                    //matrixRegistry.at(macAddressStr).connectAsync();
                    Serial.printf("Queued BLE connectie met %s\n", macAddressStr);
                }

                // --- 3. TOEWĲZEN aan de ClientState ---

                // Haal de referentie op van het (nieuwe of bestaande) apparaat
                iPixelDevice& targetDevice = matrixRegistry.at(macAddressStr);

                // Wijs de pointer in de ClientState toe aan dit apparaat
                state.assignedMatrix = &targetDevice;

                client->text("ACK: Succesvol toegewezen aan MAC: " + String(macAddressStr));

            } else if (state.assignedMatrix != nullptr) {
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



/*
            // Routering: Zoek de juiste Matrix op
            if (matrixRegistry.count(targetKey)) {
                iPixelDevice& targetDevice = matrixRegistry.at(targetKey);

                // Roep de BLE methode aan op de specifieke controller
                if (strcmp(command, "SET_COLOR") == 0) {
                    const char* colorValue = doc["value"];
                    Serial.printf("calling sendColor.\n");

 //                   targetDevice.sendColor(colorValue); // Implementeer deze in iPixelDevice
                    client->text("ACK: Kleur gestuurd naar " + String(targetKey));
                } else if (strcmp(command, "ON") == 0) {
                     Serial.printf("calling sendOnCommand.\n");
   //                  targetDevice.sendOnCommand(); // Implementeer deze
                    client->text("ACK: ON commando naar " + String(targetKey) + " verstuurd.");
                }
            } else {
                client->text("ERROR: Onbekende matrix ID.");
            }
            */
            break;
        }
    }
}

void init_webserver() {

    Serial.println("[Webserver] Initializing webserver...");

    setupBuffer() ; // reserve socket buffer

    // Registreer de event handler bij de WebSockets server
    ws.onEvent(onWsEvent);
    // Voeg de WebSockets handler toe aan de HTTP server
    server.addHandler(&ws);
    // Start de HTTP/WebSocket serverd
    server.begin();
    Serial.println("WebSocket Server gestart op ws://" + WiFi.localIP().toString() + "/ws");
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    Serial.println("[Webserver] Webserver initialized!");
}
