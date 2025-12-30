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
            }

            Serial.printf("WebSocket Client #%u verbonden.\n", client->id());
            // Voeg nieuwe client toe aan de statusmap
            clientStates[client->id()] = ClientState();
            Serial.printf("Client #%u verbonden.\n", client->id());

            //
            // start the BLE connection
            //
            const char* macAddressStr = mac.c_str() ;
            if (matrixRegistry.count(macAddressStr) == 0) {

                // --- 2. APPARAAT BESTAAT NIET: CREËER & REGISTREER DYNAMISCH ---

                // a. Converteer de string MAC-adres naar een NimBLEAddress object
                NimBLEAddress bleAddress(macAddressStr, 0);

                auto result = matrixRegistry.emplace(
                    macAddressStr,
                    iPixelDevice(bleAddress)
                );
                Serial.printf("Nieuw iPixelDevice aangemaakt voor MAC: %s\n", macAddressStr);
                iPixelDevice& targetDevice = result.first->second;
                // size MatrixContext afhankelijk van device! (hoe herkennen we dit type?)
                MatrixContext* context = new (std::nothrow) MatrixContext(32, 32, 8);
                targetDevice.context_data = static_cast<void*>(context);
                // c. Start direct de BLE verbinding voor dit nieuwe apparaat
                matrixRegistry.at(macAddressStr).connectAsync();
                Serial.printf("Queued BLE connectie met %s\n", macAddressStr);
            }

            ClientState& state = clientStates.at(client->id());
            iPixelDevice& targetDevice = matrixRegistry.at(macAddressStr);
            state.assignedMatrix = &targetDevice;
            client->text("ACK: Succesvol toegewezen aan MAC: " + String(macAddressStr));

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
			debugPrintf("Data ontvangen client: #%u, lengte %d\n", client->id(), len);

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
    server.begin();
    Serial.println("WebSocket Server gestart op ws://" + WiFi.localIP().toString() + "/ws");
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
	debugPrintf("[Webserver] Webserver initialized!\n") ;
}
