//
// Created by zweepje on 2-12-2025.
//
#include <time.h>     // Standaard C-bibliotheek voor tijdstructuren
#include "esp_sntp.h"     // ESP32-specifieke functies voor NTP
#include <Arduino.h>  // Voor Serial en String

#include "utils/webserial.h"

// Functie om de ESP32 te configureren voor NTP en te synchroniseren
void initTime() {

    Serial.println("In initTime.");
    // 1. Configureer de NTP-server(s) (tijd wordt als UTC ontvangen)
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    // 2. Stel de tijdzone in voor Nederland/België (inclusief automatische zomertijd/wintertijd)
    // "CET-1CEST,M3.5.0,M10.5.0/3"
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset(); // Pas de tijdzone aan

    Serial.println("Wachten op NTP synchronisatie...");

    // Wacht tot de tijd gesynchroniseerd is (duurt meestal een paar seconden)
    time_t now = time(nullptr);
    int attempts = 0;
    while (now < 100000 && attempts < 20) { // now < 100000 betekent dat de tijd nog niet is ingesteld
        delay(500);
        now = time(nullptr);
        attempts++;
    }

    if (now < 100000) {
        Serial.println("Tijd synchronisatie mislukt na 10 seconden.");
    } else {
        Serial.println("Tijd gesynchroniseerd!");
    }
}

/**
 * @brief Haalt de actuele, lokaal geformatteerde tijd op als HH:MM.
 * @return Een Arduino String met de tijd, of "Tijd??" als de synchronisatie mislukt.
 */
String getCurrentTimeString() {
    time_t now;
    struct tm timeinfo;

    // Vraag de huidige tijd op (seconds since epoch)
    time(&now);

    // Converteer de tijd naar de lokale tijdzone (met DST correctie)
    // De resultaten worden opgeslagen in de timeinfo structuur
    localtime_r(&now, &timeinfo);

    // Controleer op een geldige tijd (bijv. als NTP is mislukt)
    if (timeinfo.tm_year < (2020 - 1900)) {
        // Als het jaartal kleiner is dan 2020 (wat een ongesynchroniseerde tijd aangeeft)
        return "Tijd??";
    }

    // Format naar HH:MM (uur:minuut) en sla op in een C-string buffer
    char timeBuffer[6]; // Groot genoeg voor "HH:MM\0"

    // strftime formatteert de struct tm naar de gewenste string
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", &timeinfo);

    debugPrintf("Timestring is <%s>\n", timeBuffer);

    // Converteer de C-string naar een Arduino String object
    return String(timeBuffer);
}