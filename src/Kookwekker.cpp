//
// Created by zweepje on 4-1-2026.
//

#include "Kookwekker.h"
#include "utils/webserial.h"

//#include "TimerLogic.h"

// Interne variabelen voor de wekker
int timerSeconds = 0;
bool isRunning = false;
unsigned long lastTick = 0;

void processTimerCommand(StaticJsonDocument<4096>& doc) {
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

    // Optioneel: Stuur direct een status update terug naar Home Assistant
    // updateHomeAssistant();
}

void handleTimerLogic() {
    // Check of er een seconde voorbij is
    if (isRunning && millis() - lastTick >= 1000) {
        lastTick = millis();

        if (timerSeconds > 0) {
            timerSeconds--;

            // Debug output elke seconde
            int m = timerSeconds / 60;
            int s = timerSeconds % 60;
            Serial.printf("Resterend: %02d:%02d\n", m, s);
        } else {
            isRunning = false;
            Serial.println("ALARM! Tijd is om.");
            // Hier kun je een buzzer of LED-strip triggeren
        }
    }
}
