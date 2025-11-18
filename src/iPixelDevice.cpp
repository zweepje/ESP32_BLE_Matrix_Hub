#include "iPixelDevice.h"

#include <cstring>

#include "iPixelCommands.h"

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
        Serial.printf("Verwerk commando voor %s: %s\n", address.toString().c_str(), command.c_str());

        // Implementeer de daadwerkelijke BLE-schrijflogica (zie volgende stap)
        //       executeBleCommand(command);

        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, command);
        if ( error) {
                Serial.printf("JSON Deserialisatie Fout: %s\n", error.c_str());
                return;
        }

        //
        // Check which command
        //
        const char* cmd = doc["command"];
        JsonArray params = doc["params"];
        if ( strcmp( cmd, "ASSIGN" ) == 0 ) {
            //
            // make the connection
            // c. Start direct de BLE verbinding voor dit nieuwe apparaat
            connectAsync();

        } else {
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





            //String text_str = "Jezus is een vriend van mij.";
            // sendText( text_str, 1, 1, 20, 0,0,99, 0, 16 );
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
    Serial.println("Connection lijkt goed te gaan!");
    connected = true;
}

void iPixelDevice::onDisconnect(NimBLEClient *pClient) {
    printPrefix();
    Serial.println("Disconnected!");
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
    Serial.printf("Commando in queue geplaatst voor %s: %s\n",
                  address.toString().c_str(),
                  output.c_str());
}



void iPixelDevice::connectAsync() {
    if (connected) {
        printPrefix();
        Serial.println("WARN: Already connected!");
        return;
    }

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
        return;
    }

    // Discover services & characteristic
    service = client->getService(serviceUUID);
    if (!service) {
        printPrefix();
        Serial.println("ERROR: Service not found!");
        client->disconnect();
        return;
    }

    characteristic = service->getCharacteristic(charUUID);
    if (!characteristic) {
        printPrefix();
        Serial.println("ERROR: Characteristic not found!");
        client->disconnect();
        return;
    }

    connected = true;
    printPrefix();
    Serial.println("Connected successfully!");
}

void iPixelDevice::queueTick() {
    if (queue.empty()) return;
    //Get command from queue
    std::vector<uint8_t> &command = queue.front();

    //Take bytes from command
    size_t chunkSize = min(500, (int)command.size());

    //Write bytes from command
    characteristic->writeValue(command.data(), chunkSize, false);

    //Debug
    printPrefix();
    Serial.print("Sent chunk of ");
    Serial.print(chunkSize);
    Serial.print(" bytes (remaining: ");
    Serial.print(command.size());
    Serial.print(") (queue size: ");
    Serial.print(queue.size());
    Serial.println(")");

    //Print bytes as HEX
    Serial.print("Data: ");
    for (size_t i = 0; i < chunkSize; i++) {
        if (command[i] < 0x10) Serial.print('0'); // leading zero for single-digit hex
        Serial.print(command[i], HEX);
        Serial.print(' ');
    }
    Serial.println();

    //Remove bytes from command
    command.erase(command.begin(), command.begin() + chunkSize);

    //Remove command if empty
    if (command.empty()) queue.erase(queue.begin());

    //Do not overload BLE
    delay(100);
}

void iPixelDevice::queuePush(std::vector<uint8_t> command) {
    queue.push_back(command);
    printPrefix();
    Serial.print("Added command with ");
    Serial.print(command.size());
    Serial.print(" bytes to queue at ");
    Serial.println(queue.size() - 1);
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
    std::vector<uint8_t> command = iPixelCommands::sendPNG(gifData);
    printPrefix();
    Serial.print("GIF with ");
    Serial.print(gifData.size());
    Serial.println(" bytes");
    queuePush(command);
}