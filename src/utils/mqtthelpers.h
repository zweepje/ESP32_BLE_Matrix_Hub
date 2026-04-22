//
// Created by zweepje on 21-4-2026.
//

#ifndef MYIPIXEL_MQTTHELPERS_H
#define MYIPIXEL_MQTTHELPERS_H


void publish_status( int matrixid, boolean state ) ;
void publish_time(int matrixid, int hour, int minute) ;

void publishHaSensor( int matrixId, String name ) ;
void publishSensor(int matrixid, String name, String value ) ;

void publishHaNumber( int matrixId, String name ) ;
void publishNumber(int matrixid, String name, String value ) ;


#endif //MYIPIXEL_MQTTHELPERS_H