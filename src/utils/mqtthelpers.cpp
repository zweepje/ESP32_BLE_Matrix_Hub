//
// Created by zweepje on 21-4-2026.
//
#pragma once
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


