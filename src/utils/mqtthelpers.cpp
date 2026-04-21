//
// Created by zweepje on 21-4-2026.
//

#include <PubSubClient.h>

extern PubSubClient mqttClient ;

//
// publish online/offline for a matrix
//
void publish_status( int matrixid, boolean state ) {

	if ( mqttClient.publish(
	  "ESP32_KEUKEN/matrix/0/status",
	  ( state ? "online" :  "offline" ),
	  true
	) ) {
		Serial.println("     MQTT publish OK");
	} else {
		Serial.println("     MQTT Failed!!!! ");
	}
}


char * tdevicename = "ESP32_KEUKEN";

void publish_time(int matrixid, int hour, int minute) {
	char topic[100];
	char value[10];

	snprintf(topic, sizeof(topic),
		"%s/matrix/%d/time",
		tdevicename, matrixid
	);

	snprintf(value, sizeof(value),
		"%02d:%02d",
		hour, minute
	);

	mqttClient.publish(topic, value, true);
}
