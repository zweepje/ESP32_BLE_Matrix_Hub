//
// Created by zweepje on 29-11-2025.
//

#ifndef MYIPIXEL_TEMPERATURE_H
#define MYIPIXEL_TEMPERATURE_H

//bool make_temperature( String& resultString, float temperature, String title ) ;
bool make_animated_temperature( std::vector<uint8_t>& binaryDataVector, float temperature, String title );
#endif //MYIPIXEL_TEMPERATURE_H