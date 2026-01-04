//
// Created by zweepje on 28-12-2025.
//
#include <cstdio>
#include <esp32-hal.h>
#include <WebSerial.h>
#include <WString.h>

// Functie prototype
void debugPrintf(const char *format, ...) {


    char loc_res[256]; // Buffer voor de geformatteerde tekst
    va_list arg;
    va_list copy;

    va_start(arg, format);
    va_copy(copy, arg);

    // Bereken hoe groot de string gaat worden
    int len = vsnprintf(nullptr, 0, format, copy);
    va_end(copy);

    if (len > 0) {
        char *buffer = (char *)malloc(len + 1);
        if (buffer) {
            vsnprintf(buffer, len + 1, format, arg);

            // 1. Stuur naar de fysieke Seriële poort
            Serial.print(buffer);

            // 2. Stuur naar de WebSerial interface
           WebSerial.print(buffer);

            free(buffer);
        }
    }
    va_end(arg);

}


String getLocalTimestamp() {
    unsigned long allSeconds = millis() / 1000;
    int runMillis = millis() % 1000;
    int runSeconds = allSeconds % 60;
    int runMinutes = (allSeconds / 60) % 60;

    char buf[16];
    // Formaat: Minuten:Seconden.Milliseconden (bijv. 04:22.450)
    snprintf(buf, sizeof(buf), "%02d:%02d.%03d", runMinutes, runSeconds, runMillis);
    return String(buf);
}