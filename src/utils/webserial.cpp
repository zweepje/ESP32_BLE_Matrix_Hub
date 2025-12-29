//
// Created by zweepje on 28-12-2025.
//
#include <WebSerial.h>

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
            //WebSerial._flush_print_buffer();

            free(buffer);
        }
    }
    va_end(arg);
}