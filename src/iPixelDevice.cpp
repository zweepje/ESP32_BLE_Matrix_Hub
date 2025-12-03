#include "iPixelDevice.h"
#include "global.h"
#include <cstring>

#include "global.h"
#include "iPixelCommands.h"
#include "Helpers.h"
#include "functions/temperature.h"
#include "clock/timefunctions.h"

NimBLEUUID serviceUUID("000000fa-0000-1000-8000-00805f9b34fb");
NimBLEUUID charUUID("0000fa02-0000-1000-8000-00805f9b34fb");

extern std::map<std::string, iPixelDevice> matrixRegistry;



void iPixelDevice::processQueue() {

    // 1. Controleer of er verbinding is en of de queue niet leeg is
    //   if (!commandQueue.empty() && isBLEConnected()) {
    if (!commandQueue.empty() ) {
        // Haal het oudste commando op
        std::string command = commandQueue.front();

        // Verwijder het commando uit de queue *voordat* we de BLE actie starten
        // Dit voorkomt dat we het commando opnieuw versturen bij een BLE-fout
        commandQueue.pop();

        // Verwerk en verstuur het commando
//        DBG_PRINTF( DEBUG_QUEUE, "Verwerk commando voor %s: %s\n", address.toString().c_str(), command.c_str());

        // Implementeer de daadwerkelijke BLE-schrijflogica (zie volgende stap)
        //       ex
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        StaticJsonDocument<4096> doc; // of StaticJsonDocument<CAPACITY> doc;
#pragma GCC diagnostic pop

        DeserializationError error = deserializeJson(doc, command);
        if ( error) {
            DBG_PRINTF( DEBUG_QUEUE, "JSON Deserialisatie Fout: %s\n", error.c_str());
            return;
        }

        //
        // Check which command
        //
        const char* cmd = doc["command"];
        JsonArray params = doc["params"];
//        DBG_PRINTF( DEBUG_QUEUE, "JSON Command: %s\n", cmd );
        if ( strcmp( cmd, "ASSIGN" ) == 0 ) {



            //
            // make the connection
            // c. Start direct de BLE verbinding voor dit nieuwe apparaat
            connectAsync();

        } else if  ( strcmp( cmd, "send_text" ) == 0 ) {

            const char* text_str = params[0];

            // fill in defaults
            int animation = 1;
            int save_slot = 1;
            int speed = 50; // Voorbeeld default
            uint8_t colorR = 255, colorG = 255, colorB = 255; // Default wit
            int rainbow_mode = 0;
            int matrix_height = 16; // Voorbeeld default

            // Loop door de array vanaf het tweede element (index 1)
            for (size_t i = 1; i < params.size(); i++) {
                String paramStr = params[i].as<String>();
                int equals_index = paramStr.indexOf('=');

                if (equals_index > 0) {
                    String key = paramStr.substring(0, equals_index);
                    String value = paramStr.substring(equals_index + 1);

                    if (key.equalsIgnoreCase("animation")) {
                        animation = value.toInt();
                    } else if (key.equalsIgnoreCase("save_slot")) {
                        save_slot = value.toInt();
                    } else if (key.equalsIgnoreCase("speed")) {
                        speed = value.toInt();
                    } else if (key.equalsIgnoreCase("rainbow_mode")) {
                        rainbow_mode = value.toInt();
                    } else if (key.equalsIgnoreCase("matrix_height")) {
                        matrix_height = value.toInt();
                    } else if (key.equalsIgnoreCase("color")) {
                        // Speciale handling voor Hex (RRGGBB)
                        if (value.length() == 6) {
                            // Konverteren van Hex-string naar 8-bit getallen
                            colorR = (uint8_t)strtol(value.substring(0, 2).c_str(), NULL, 16);
                            colorG = (uint8_t)strtol(value.substring(2, 4).c_str(), NULL, 16);
                            colorB = (uint8_t)strtol(value.substring(4, 6).c_str(), NULL, 16);
                        }
                    }
                }
            }

            // 4. Roep de functie aan met de geparseerde/default waarden
            this->sendText(text_str, animation, save_slot, speed, colorR, colorG, colorB, rainbow_mode, matrix_height);

        } else if  ( strcmp( cmd, "make_time_ani" ) == 0 ) {

  //          DBG_PRINTF( DEBUG_QUEUE, "calling make_animated time\n");
            std::vector<uint8_t> binaryDataVector;
            String time = getCurrentTimeString();
            make_animated_time( binaryDataVector, time ) ;
            this->sendGIF( binaryDataVector );


        } else if  ( strcmp( cmd, "make_temperature_ani" ) == 0 ) {
            // parse parameters
            const char* text_str = params[0];
            // fill in defaults
            float temperature = 10.0f;
            String title = "wc";

            // LETOP: begint hier met 0!!!
            for (size_t i = 0; i < params.size(); i++) {
                String paramStr = params[i].as<String>();
                int equals_index = paramStr.indexOf('=');

//                Serial.printf("paramStr %s: ", paramStr.c_str() ) ;



                if (equals_index > 0) {
                    String key = paramStr.substring(0, equals_index);
                    String value = paramStr.substring(equals_index + 1);
 //                   Serial.printf("key %s: ", key.c_str() ) ;

                    if (key.equalsIgnoreCase("temperature")) {
                        temperature = value.toFloat();
 //                       Serial.printf("detected temp %f ", temperature ) ;

                    } else if (key.equalsIgnoreCase("title")) {
                        title = value;
                    }
                }
            }
 //           DBG_PRINTF( DEBUG_QUEUE, "calling make_animated temperature\n");
            std::vector<uint8_t> binaryDataVector;
            make_animated_temperature( binaryDataVector, temperature, title ) ;
            this->sendGIF( binaryDataVector );



        } else if  ( strcmp( cmd, "send_gif" ) == 0 ) {

 //           DBG_PRINTF( DEBUG_QUEUE, "Decoded send_gif commando");

            //String paramStr = params[0].as<String>();
            //this->sendPNG( Helpers::hexStringToVector(paramStr) );
            std::vector<uint8_t> binaryDataVector;
            make_animated_temperature( binaryDataVector, 12.3, "boven" ) ;


       /*     Serial.print("make_temperature De lengte van de String is: ");
            Serial.println(aap.length());  // print de lengte van de String als getal

            std::vector<uint8_t> binaryDataVector;
            size_t len = aap.length();
            binaryDataVector.resize(len);
            memcpy(binaryDataVector.data(), aap.c_str(), len);
        */
//            Serial.print("make_temperature De lengte van de String is: ");
 //           Serial.println(binaryDataVector.size());  // print de lengte van de String als getal

            // printout string
            for ( int i=0 ; i<binaryDataVector.size() ; i++ ) {

//                Serial.printf("%02x", binaryDataVector[i]);
            }


            this->sendGIF( binaryDataVector );
            //this->sendPNG( (aap) );

            //
            // original:
            //            this->sendPNG( Helpers::hexStringToVector(paramStr) );

        }
    }
}



void iPixelDevice::printPrefix() {
    Serial.print("[iPixelDevice] [");
    Serial.print(address.toString().c_str());
    Serial.print("] ");
}

void iPixelDevice::onConnect(NimBLEClient *pClient) {

    printPrefix();

    if ( client == pClient ) {
        Serial.println("############### OnConnect matching! ###############");
    } else {
        Serial.println("############### OnConnect mismatch! ###############");
    }
    Serial.println("On Connect, connection lijkt goed te zijn!");
    connected = true;
}

void iPixelDevice::onDisconnect(NimBLEClient *pClient) {
    printPrefix();
    Serial.println("############### Disconnected! ###############");
    connected = false;
}


#include "iPixelDevice.h" // Inclusief de klassedefinitie

// Implementatie van enqueueCommand
void iPixelDevice::enqueueCommand(const JsonDocument& doc) {

    // 1. Maak een string aan waarin we de JSON opslaan
    // String (Arduino String) is handig voor serialisatie
    String output;

    // 2. Serialiseer de JSON-document (die u in onWsEvent heeft geparsd)
    // naar de string 'output'
    serializeJson(doc, output);

    // 3. Plaats de geserialiseerde string in de interne std::queue
    // We gebruiken output.c_str() om een C-stijl string te verkrijgen,
    // maar std::string is flexibeler en beter in geheugenbeheer.
    commandQueue.push(output.c_str());

    // Optioneel: Loggen om te controleren of de data in de queue is gekomen
//    Serial.printf("Commando in queue geplaatst voor %s: %s\n",
//                  address.toString().c_str(),
//                  output.c_str());
}



void iPixelDevice::connectAsync() {
    if (connected) {
        printPrefix();
        Serial.println("WARN: Already connected!");
        return;
    }

    if (g_isBleBusy) {
        // Iemand anders is bezig met BLE, probeer later opnieuw
        Serial.println("WARN: BLE locked");
        return ;
    }

    g_isBleBusy = true; // Neem de lock

    printPrefix();
    Serial.println("Connecting...");

    if (!client) {
        Serial.println("Creating client...");
        client = NimBLEDevice::createClient();
        client->setClientCallbacks(this, false);
    }

    // Async connect (non-blocking)
    if (!client->connect(address)) {
        printPrefix();
        Serial.println("ERROR: Failed to connect!");
        g_isBleBusy = false;
        return;
    }

    // Discover services & characteristic
    service = client->getService(serviceUUID);
    if (!service) {
        printPrefix();
        Serial.println("ERROR: Service not found!");
        client->disconnect();
        g_isBleBusy = false;
        return;
    }

    characteristic = service->getCharacteristic(charUUID);
    if (!characteristic) {
        printPrefix();
        Serial.println("ERROR: Characteristic not found!");
        client->disconnect();
        g_isBleBusy = false;
        return;
    }

    g_isBleBusy = false;
    connected = true;
    printPrefix();
    Serial.println("Connected successfully!");
}

//
// hier wordt de BLE queue verwerkt
//
void iPixelDevice::queueTick() {
    if (queue.empty()) return;

    if (g_isBleBusy) {
        // Queue is verwerkt, maar kan nu niet schrijven.
        // Optioneel: Zet commando terug in queue of wacht.
        return;
    }

    g_isBleBusy = true; // Neem de lock


    //Get command from queue
    std::vector<uint8_t> &command = queue.front();

    //Take bytes from command
    size_t chunkSize = min(500, (int)command.size());

    //Serial.print("Char is " ) ;
    //Serial.println((unsigned long)characteristic, HEX);

    if ( this->client != nullptr && client->isConnected()) {


    //Write bytes from command
    characteristic->writeValue(command.data(), chunkSize, false);

    //Debug
 //   printPrefix();

  //  DBG_PRINTF( DEBUG_BLE, "Sent chunk of ");

  //  DBG_PRINTF( DEBUG_BLE,"ChunkSize: %u\n", chunkSize);


    //DBG_PRINTF( DEBUG_BLE," bytes (remaining:  %)\n", command.size() );
    //DBG_PRINTF( DEBUG_BLE, " (queue size:%u)", queue.size() );
    //Print bytes as HEX
    DBG_PRINTF( DEBUG_BLE2,"Data: ");
    for (size_t i = 0; i < chunkSize; i++) {
        if (command[i] < 0x10) DBG_PRINTF( DEBUG_BLE2,"0" ); // leading zero for single-digit hex
          DBG_PRINTF( DEBUG_BLE2, "02x", command[i] );
          DBG_PRINTF( DEBUG_BLE2, " ");
    }
    //Serial.println();

    //Remove bytes from command
    command.erase(command.begin(), command.begin() + chunkSize);

    //Remove command if empty
    if (command.empty()) queue.erase(queue.begin());

    //Do not overload BLE
    delay(100);
    } else {
        // Foutopsporing: print welke Matrix niet verbonden is
        Serial.printf("Matrix %s: Queue niet verwerkt. Client (0x%p) verbonden: %s\n",
                      address.toString().c_str(),
                      this->client,
                      (this->client && this->client->isConnected() ? "JA" : "NEE"));
    }
    g_isBleBusy = false; // release de lock

}

void iPixelDevice::queuePush(std::vector<uint8_t> command) {
    queue.push_back(command);
//    printPrefix();
//    Serial.print("Added command with ");
//    Serial.print(command.size());
//    Serial.print(" bytes to queue at ");
//    Serial.println(queue.size() - 1);
}

void iPixelDevice::sendImage() {
    std::vector<uint8_t> command = iPixelCommands::sendImage();
//    printPrefix();
    queuePush(command);
}

void iPixelDevice::setTime(int hour, int minute, int second) {
    std::vector<uint8_t> command = iPixelCommands::setTime(hour, minute, second);
    printPrefix();
    Serial.print("Time ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.print(second);
    Serial.println();
    queuePush(command);
}

void iPixelDevice::setFunMode(bool value) {
    std::vector<uint8_t> command = iPixelCommands::setFunMode(value);
    printPrefix();
    Serial.print("Fun Mode ");
    Serial.print(value);
    Serial.println();
    queuePush(command);
}

void iPixelDevice::setOrientation(int orientation) {
    std::vector<uint8_t> command = iPixelCommands::setOrientation(orientation);
    printPrefix();
    Serial.print("Orientation ");
    Serial.print(orientation);
    Serial.println();
    queuePush(command);
}

void iPixelDevice::clear() {
    std::vector<uint8_t> command = iPixelCommands::clear();
    printPrefix();
    Serial.print("Clear");
    Serial.println();
    queuePush(command);
}

void iPixelDevice::setBrightness(int brightness) {
    std::vector<uint8_t> command = iPixelCommands::setBrightness(brightness);
    printPrefix();
    Serial.print("Brightness ");
    Serial.print(brightness);
    Serial.println();
    queuePush(command);
}

void iPixelDevice::setSpeed(int speed) {
    std::vector<uint8_t> command = iPixelCommands::setSpeed(speed);
    printPrefix();
    Serial.print("Speed ");
    Serial.print(speed);
    Serial.println();
    queuePush(command);
}

void iPixelDevice::setLED(boolean on) {
    std::vector<uint8_t> command = iPixelCommands::setLED(on);
    printPrefix();
    Serial.print("LED: ");
    Serial.println(on ? "On" : "Off");
    queuePush(command);
}

void iPixelDevice::deleteScreen(int screen) {
    std::vector<uint8_t> command = iPixelCommands::deleteScreen(screen);
    printPrefix();
    Serial.print("Delete Screen ");
    Serial.print(screen);
    Serial.println();
    queuePush(command);
};

void iPixelDevice::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    std::vector<uint8_t> command = iPixelCommands::setPixel(x, y, r, g, b);
    printPrefix();
    Serial.print("Pixel at (");
    Serial.print(x);
    Serial.print(",");
    Serial.print(y);
    Serial.print(") to rgb(");
    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.print(b);
    Serial.print(")");
    Serial.println();
    queuePush(command);
};

void iPixelDevice::setClockMode(int style, int dayOfWeek, int year, int month, int day, bool showDate, bool format24) {
    std::vector<uint8_t> command = iPixelCommands::setClockMode(style, dayOfWeek, year, month, day, showDate, format24);
    printPrefix();
    Serial.print("Clock Mode: style=");
    Serial.print(style);
    Serial.print(", showDate=");
    Serial.print(showDate);
    Serial.print(", format24=");
    Serial.print(format24);
    Serial.print(", date=");
    Serial.print(day);
    Serial.print("-");
    Serial.print(month);
    Serial.print("-");
    Serial.print(year);
    Serial.print(", dayOfWeek=");
    Serial.print(dayOfWeek);
    Serial.println();
    queuePush(command);
};

void iPixelDevice::setRhythmLevelMode(int style, const int levels[11]) {
    std::vector<uint8_t> command = iPixelCommands::setRhythmLevelMode(style, levels);
    printPrefix();
    Serial.print("Rythm Level Mode: Style ");
    Serial.print(style);
    Serial.print(" with Levels ");
    for(int i=0; i<11; i++) {
        if(i!=0) Serial.print(",");
        Serial.print(levels[i]);
    }
    Serial.println();
    queuePush(command);
}

void iPixelDevice::setRhythmAnimationMode(int style, int frame) {
    std::vector<uint8_t> command = iPixelCommands::setRhythmAnimationMode(style, frame);
    printPrefix();
    Serial.print("Rythm Animation Mode: Style ");
    Serial.print(style);
    Serial.print(" at Frame ");
    Serial.print(frame);
    Serial.println();
    queuePush(command);

};

void iPixelDevice::sendText(const String& text, int animation, int save_slot, int speed, uint8_t colorR, uint8_t colorG, uint8_t colorB, int rainbow_mode, int matrix_height) {
    std::vector<uint8_t> command = iPixelCommands::sendText(text, animation, save_slot, speed, colorR, colorG, colorB, rainbow_mode, matrix_height);
    printPrefix();
    Serial.print("Text: text=");
    Serial.print(text);
    Serial.print(", animation=");
    Serial.print(animation);
    Serial.print(", save_slot=");
    Serial.print(save_slot);
    Serial.print(", speed=");
    Serial.print(speed);
    Serial.print(", color=(");
    Serial.print(colorR);
    Serial.print(",");
    Serial.print(colorG);
    Serial.print(",");
    Serial.print(colorB);
    Serial.print("), rainbow_mode=");
    Serial.print(rainbow_mode);
    Serial.print(", matrix_height=");
    Serial.println(matrix_height);
    queuePush(command);
};

void iPixelDevice::sendPNG(const std::vector<uint8_t> &pngData) {
    std::vector<uint8_t> command = iPixelCommands::sendPNG(pngData);
    printPrefix();
    Serial.print("PNG with ");
    Serial.print(pngData.size());
    Serial.println(" bytes");
    queuePush(command);
}


void iPixelDevice::sendGIF(const std::vector<uint8_t> &gifData) {
    std::vector<uint8_t> command = iPixelCommands::sendGIF(gifData);
//    printPrefix();
//    Serial.print("GIF with ");
 //   Serial.print(gifData.size());
 //   Serial.println(" bytes");
    queuePush(command);
}