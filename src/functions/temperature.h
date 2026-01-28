//
// Created by zweepje on 29-11-2025.
//

#ifndef MYIPIXEL_TEMPERATURE_H
#define MYIPIXEL_TEMPERATURE_H

//bool make_temperature( String& resultString, float temperature, String title ) ;
bool make_animated_temperature( void* generic_context, std::vector<uint8_t>& binaryDataVector, float temperature, String title );
bool make_animated_time( void* generic_context, std::vector<uint8_t>& binaryDataVector, String time);
bool make_kooktime( void* generic_context, std::vector<uint8_t>& binaryDataVector, int time );
bool make_clock( std::vector<uint8_t>& binaryDataVector, int hour, int min, int seconds ) ;
bool make_temp_graph( void* generic_context, std::vector<uint8_t>& binaryDataVector, float kamertemp[24], float buitentemp[24] );


#endif //MYIPIXEL_TEMPERATURE_H