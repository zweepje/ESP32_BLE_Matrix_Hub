#include "iPixelDevice.h"
#include "global.h"
#include <cstring>
#include "main.h"

#include "global.h"
#include "iPixelCommands.h"
#include "Helpers.h"
#include "functions/temperature.h"
#include "clock/timefunctions.h"
#include "utils/webserial.h"

NimBLEUUID serviceUUID("000000fa-0000-1000-8000-00805f9b34fb");
NimBLEUUID charUUID("0000fa02-0000-1000-8000-00805f9b34fb");

extern std::map<std::string, iPixelDevice> matrixRegistry;



void iPixelDevice::processTimerCommand(StaticJsonDocument<4096>& doc) {

    const char* action = doc["action"];

    debugPrintf("TimerCommand received <%s>\n", action );


    if (strcmp(action, "INC_MIN") == 0) {
        timerSeconds += 60;
        Serial.printf("Tijd toegevoegd: %d sec\n", timerSeconds);
    }
    else if (strcmp(action, "INC_SEC") == 0) {
        timerSeconds += 1;
    }
    else if (strcmp(action, "TOGGLE") == 0) {
        isRunning = !isRunning;
        Serial.printf("Wekker %s\n", isRunning ? "gestart" : "gepauzeerd");
    }
    else if (strcmp(action, "RESET") == 0) {
        timerSeconds = 0;
        isRunning = false;
        Serial.println("Wekker gereset.");
    }
    else return ; // nothing changed

    showTime( timerSeconds );
    // Optioneel: Stuur direct een status update terug naar Home Assistant
    // updateHomeAssistant();
}



void iPixelDevice::showTime( int timerSeconds ) {

    int m = timerSeconds / 60;
    int s = timerSeconds % 60;
    Serial.printf("Resterend: %02d:%02d\n", m, s);

    std::vector<uint8_t> binaryDataVector;
    char timeBuffer[6]; // Ruimte voor "mm.ss" + de afsluitende '\0'
    snprintf(timeBuffer, sizeof(timeBuffer), "%02d.%02d", m, s);

    String displayTime = String(timeBuffer);

    debugPrintf("Timestring is <%s>\n", displayTime.c_str() );

    make_kooktime( this->context_data,binaryDataVector, displayTime ) ;
    debugPrintf("Sending GIF <%s>\n", displayTime.c_str() );

    this->sendGIF( binaryDataVector );

}


void iPixelDevice::handleTimerLogic() {

    // Check of er een seconde voorbij is
    if (isRunning && millis() - lastTick >= 1000) {
        lastTick = millis();

        if (timerSeconds > 0) {
            timerSeconds--;
            showTime( timerSeconds );

        } else {
            isRunning = false;
            Serial.println("ALARM! Tijd is om.");
            // Hier kun je een buzzer of LED-strip triggeren
        }
    }
}

void iPixelDevice::update() {

    switch (_state) {
        case IDLE:
            // Niets doen, we wachten op een commando om te verbinden
            break;

        case CONNECTING:
            // We wachten hier simpelweg tot de NimBLE-callback de status verandert.
            // Je zou hier eventueel een 'timeout' kunnen toevoegen.
            break;

        case WAITING_FOR_POST:
            // De callback heeft gemeld dat er verbinding is.
            // Nu voeren we de zware taken uit in de hoofd-loop:
            postconnect();
            // postConnectAsync zet de state op READY als hij klaar is
            break;

        case READY:
            // Hier gebeurt het echte werk:
            // Bijvoorbeeld: check of er nieuwe data in de buffer zit om te verzenden.
            processQueue();
            break;

        case ERROR:
            // Foutafhandeling: probeer bijvoorbeeld na 5 seconden opnieuw te verbinden
            //if (millis() - _lastErrorTime > 5000) {
            //    Serial.println("Retry verbinding na fout...");
            //    connectAsync();
            //}
            Serial.printf("Error iPixelDevice\n" );
            break;
    }
}
//
// ProcessQueue ( data ontvangen door WS )
//
void iPixelDevice::processQueue() {


    //Serial.write("ProcessQueue: " );

    // 1. Controleer of er verbinding is en of de queue niet leeg is
    //   if (!commandQueue.empty() && isBLEConnected()) {
    if (!commandQueue.empty() ) {
        Serial.printf("data available." );

        // Haal het oudste commando op
        std::string command = commandQueue.front();

        // Verwijder het commando uit de queue *voordat* we de BLE actie starten
        // Dit voorkomt dat we het commando opnieuw versturen bij een BLE-fout
        commandQueue.pop();

        // Verwerk en verstuur het commando
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
#ifdef KOOKWEKKER
        processTimerCommand( doc );
        return;
#endif


        // todo: cmd kan null zijn!!
        const char* cmd = doc["command"];
        JsonArray params = doc["params"];
        debugPrintf(  "JSON Command: %s\n", cmd );

       if  ( strcmp( cmd, "send_text" ) == 0 ) {

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

        } else if  ( strcmp( cmd, "set_brightness" ) == 0 ) {

            String br_str = params[0].as<String>();
            int brightness = br_str.toInt();
            Serial.printf("Set brightness to: %d", brightness ) ;
            this->setBrightness( brightness );

        } else if  ( strcmp( cmd, "make_time_ani" ) == 0 ) {

  //          DBG_PRINTF( DEBUG_QUEUE, "calling make_animated time\n");
            std::vector<uint8_t> binaryDataVector;
            String time = getCurrentTimeString();
            make_animated_time( this->context_data,binaryDataVector, time ) ;
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
            make_animated_temperature( this->context_data, binaryDataVector, temperature, title ) ;
            this->sendGIF( binaryDataVector );



        } else if  ( strcmp( cmd, "send_gif" ) == 0 ) {

            debugPrintf( "[iPixelDevice]Decoded send_gif commando\n");

            String paramStr = params[0].as<String>();
            debugPrintf("Paramstring size %d- %d %d %d", paramStr.length(), paramStr[0], paramStr[1], paramStr[2]);

            debugPrintf("Dit komt uit de queue als params[0]:\n");
            for ( int i=0 ; i<20 ; i++ ) {
                debugPrintf( "%c", paramStr[i] );
            }
            debugPrintf(("\n"));


            //debugPrintf("params %s\n", paramStr.c_str());
            this->sendGIF( Helpers::hexStringToVector(paramStr) );
            debugPrintf("[iPixelDevice] gif verzonden");
        //    std::vector<uint8_t> binaryDataVector;
        //    make_animated_temperature( this->context_data,binaryDataVector, 12.3, "boven" ) ;


        }


   }

    else {
        //Serial.printf("Queue empty" );
        delay( 100 ) ;

    }

}



void iPixelDevice::printPrefix() {
    Serial.print("[iPixelDevice] [");
    Serial.print(address.toString().c_str());
    Serial.print("] ");
}


//
// callback when connection is made BLE
//
void iPixelDevice::onConnect(NimBLEClient *pClient) {

    printPrefix();

    if ( client == pClient ) {
        Serial.println("############### OnConnect matching! ###############");
        dopostconnect = true ;
    } else {
        Serial.println("############### OnConnect mismatch! ###############");
    }
    pClient->updateConnParams( 20, 40, 0, 400);
    _state = WAITING_FOR_POST;

   // postconnectAsync() ;
   // Serial.println("On Connect, connection lijkt goed te zijn!");


}

void iPixelDevice::onConnectFail(NimBLEClient* pClient, int reason) {

    Serial.println("############### Connection Failed! ###############");
    Serial.printf("Reason %d\n", reason);
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
//    debugPrintf("Commando in queue geplaatst voor %s: %s\n",
//                  address.toString().c_str(),
//                 output.c_str());
}



void iPixelDevice::connectAsync() {


    if (connected) {
        printPrefix();
        Serial.println("WARN: Already connected!");
        return;
    }

    _state = CONNECTING;
    printPrefix();
    debugPrintf("[iPixelDevice] Connecting with %s",address.toString().c_str());

    if (!client) {
        Serial.println("Creating client...");
        client = NimBLEDevice::createClient();
        client->setClientCallbacks(this, false);
    }

    // Async connect (non-blocking)
    bool status = client->connect( address,true,true, true ) ;
    //bool status = client->connect( address) ;  // blocking
    if ( !status ) {

        printPrefix();
        Serial.println("ERROR: Failed to start BLE connection!");
        return;
    }
    connecting = true;
}

void iPixelDevice::postconnect() {

    Serial.println("In Post connect");
    // Discover services & characteristic
    service = client->getService(serviceUUID);
    if (!service) {
        printPrefix();
        debugPrintf("ERROR: Service not found!\n");
        client->disconnect();
        return;
    }


    characteristic = service->getCharacteristic(charUUID);
    if (!characteristic) {
        printPrefix();
        debugPrintf("ERROR: Characteristic not found!\n");
        client->disconnect();
        return;
    }

    // 14 Dec Optimalisatie: MTU bepalen
    uint16_t mtu = client->getMTU();
    // De chunksize is MTU - 3 bytes overhead
    this->chunkSize = mtu - 3;

    connected = true;
    connecting = false;
    dopostconnect = false;
    debugPrintf("[iPixelDevice] Connected with %s\n",address.toString().c_str());
    debugPrintf("[iPixelDevice] MTU is %d, chunksize %d\n", mtu, chunkSize);
    _state = READY;  // ready for commands

}

//
// hier wordt de BLE queue verwerkt
//
void iPixelDevice::queueTick() {
    if (queue.empty()) {
        //Serial.println( "BLE queue is empty");
        return;
    }

    if ( !connected ) {

        Serial.println("BLE not connected");
        // remove all from queue
        queue.erase(queue.begin());
        return ;
    }


    //Get command from queue
    std::vector<uint8_t> &command = queue.front();

    Serial.printf("\n%s - Processing BLE queue\n", getLocalTimestamp().c_str() ) ;
    while ( command.size() > 0 ) {

        //Take bytes from command
        size_t chunks = min((int)this->chunkSize, (int)command.size());
        Serial.printf("Chunk is %d", chunks ) ;

        if ( this->client != nullptr && client->isConnected()) {

            bool success = characteristic->writeValue(command.data(), chunks, true);
            if (success) {
                // 2. Alleen verwijderen als het ECHT is aangekomen
                command.erase(command.begin(), command.begin() + chunks);

                if (command.empty()) {
                    queue.erase(queue.begin());
                }

                // Omdat 'true' wacht op de overkant, heb je vaak minder delay nodig
                // Maar een kleine pauze (bijv. 5-10ms) houdt de CPU koel
                delay(5);
            } else {
                // 3. Foutafhandeling: Niet wissen!
                // Bij de volgende loop probeert hij ditzelfde pakketje gewoon opnieuw.
                Serial.println("BLE Write failed, retrying next loop...");

                // Geef de verbinding even rust om te herstellen van de hik
                delay(20);
            }



            /*


                //Write bytes from command
                characteristic->writeValue(command.data(), chunks, false);

                //Remove bytes from command
                command.erase(command.begin(), command.begin() + chunks);

                    //Remove command if empty
                if (command.empty()) queue.erase(queue.begin());

                //Do not overload BLE
                //delay(100);
                */
        } else {
            // Foutopsporing: print welke Matrix niet verbonden is
            Serial.printf("Matrix %s: Queue niet verwerkt. Client (0x%p) verbonden: %s\n",
                          address.toString().c_str(),
                          this->client,
                          (this->client && this->client->isConnected() ? "JA" : "NEE"));
            delay(100);
            /*
            if ( !this->client->isConnected() ) {
                this->connected =false;
                Serial.printf("===> Matrix %s: retrying connection\n",address.toString().c_str());
                this->client->disconnect();
                this->connectAsync();
            }
            */
        }
    }
    debugPrintf("\n");
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

    debugPrintf("sendGIF %02x %02x %02x\n", gifData.at(0), gifData.at(1), gifData.at(2));
    std::vector<uint8_t> command = iPixelCommands::sendGIF(gifData);
    printPrefix();
    Serial.print("GIF with ");
    Serial.print(gifData.size());
    Serial.println(" bytes");

    Serial.printf("%2x%2x%2x", gifData[0], gifData[1], gifData[2]);
    queuePush(command);
}