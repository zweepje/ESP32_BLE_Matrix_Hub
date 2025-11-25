//
// Created by zweepje on 19-11-2025.
//

#ifndef MYIPIXEL_GLOBAL_H
#define MYIPIXEL_GLOBAL_H
#include <cstdint>

// Globale vlag/mutex om BLE-operaties te synchroniseren
extern bool g_isBleBusy;


enum DebugGroups {
    DEBUG_NONE      = 0,
    DEBUG_BLE       = (1 << 0), // 1 (0000 0001)
    DEBUG_WS        = (1 << 1), // 2 (0000 0010)
    DEBUG_QUEUE     = (1 << 2), // 4 (0000 0100)
    DEBUG_JSON      = (1 << 3), // 8 (0000 1000)
    DEBUG_WIFI      = (1 << 4), //
    DEBUG_BLE2      = (1 << 5),
    DEBUG_ALL       = 0xFF      // 255 (Alles aan)
};

// De globale variabele die bepaalt wat geprint wordt
extern uint8_t g_debugFlags;


// Macro om de debug-output conditioneel te printen
#define DBG_PRINTF(flag, format, ...) \
do { \
if (g_debugFlags & flag) { \
Serial.printf("[%s] " format, #flag, ##__VA_ARGS__); \
} \
} while (0)
// #flag converteert de enum naam (bijv. DEBUG_BLE) naar de string "DEBUG_BLE" voor de prefix.


#endif //MYIPIXEL_GLOBAL_H