//
// Created by zweepje on 21-4-2026.
//

#include <PubSubClient.h>
#include "global.h"
#include "utils/webserial.h"

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


void publishSensor(int matrixid, String name, String value ) {

	char topic[100];
	char result[10];

	snprintf(topic, sizeof(topic),
		"%s/matrix/%d/%s",
		tdevicename, matrixid, name.c_str()
	);

	snprintf(result, sizeof(result), "%s", value.c_str() ) ;

	mqttClient.publish(topic, result, true);

	debugPrintf("Publised %s, with %s\n", topic, result );

}


void publishNumber(int matrixid, String name, String value ) {

	publishSensor(matrixid, name, value );
}



void publishHaSensor( int matrixId, String name ) {

	char finalPayload[512];
	char topic[100];
	int id = matrixId;

	const char* payload1 = R"({
		"name": "Matrix %d %s",
		"state_topic": "%s/matrix/%d/%s",
		"unique_id": "%s_matrix_%d_%s",
		"device": {
			"identifiers": ["%s"],
			"name": "%s",
			"model": "ESP32 Matrix Controller"
			}
		})";

	snprintf(finalPayload, sizeof(finalPayload),
		payload1,
		id, name.c_str(),
		tdevicename, id, name.c_str(),
		tdevicename, id, name.c_str(),
		tdevicename,
		tdevicename
	);

	snprintf(topic, sizeof(topic),
		"homeassistant/sensor/%s_matrix_%d_%s/config",
		tdevicename, id, name.c_str()
	);

	mqttClient.publish(topic, finalPayload, true);
	publishSensor(id, name, "01:33" );

	Serial.println("---- TIME CONFIG ----");
	Serial.println(finalPayload);
	Serial.println("---------------------");



}

void publishHaNumber( int matrixId, String name ) {

	char finalPayload[512];
	char topic[150];
	int id = matrixId;

	const char* payload = R"({
		"name": "matrix %d %s",
		"command_topic": "%s/matrix/%d/set",
		"state_topic": "%s/matrix/%d/%s",
		"unit_of_measurement": "sec",
		"min": 0,
		"max": 7200,
		"step": 1,
		"mode": "box",
		"unique_id": "%s_matrix_%d_%s",
		"device": {
			"identifiers": ["%s"],
			"name": "%s"
		}
	})";

	snprintf(finalPayload, sizeof(finalPayload),
		payload,
		id, name.c_str(),				// name
		tdevicename, id, 				// command topic
		tdevicename, id, name.c_str(),	// state topic
		tdevicename, id, name.c_str(),	// unique id
		tdevicename,
		tdevicename
	);


	snprintf(topic, sizeof(topic),
	  "homeassistant/number/%s_matrix_%d_%s/config",
	  tdevicename, id, name.c_str()
	);

	mqttClient.publish(topic, finalPayload, true);
	publishNumber(id, name, "222" );

	Serial.println("---- Number CONFIG ----");
	Serial.println(finalPayload);
	Serial.println("---------------------");



}


