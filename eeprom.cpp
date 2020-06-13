#include "Arduino.h"
#include <EEPROM.h>
#include "eeprom.h"

Eeprom::Eeprom()
{
}

void Eeprom::setUuid(String uuid)
{
  // This will eventually come from the API.
  for (int i = 0; i < 37; i++)
  {
    Serial.print("Writing byte: ");    Serial.print(i);Serial.println("= " + String(uuid[i]));
    EEPROM.write(i, uuid[i]);
  }

  char data[37];
  for (int i = 0; i < 37; i++)
  {
   data[i] = EEPROM.read(i);
    
  }
  Serial.println(String(data));
} 

String   Eeprom::getUuid()
{
//  
//  for (int i = 0; i < 37; i++)
//  {
//    _data[i] = EEPROM.read(i);
//    
//  }
//  Serial.println(_data);
}


bool Eeprom::hasUuid()
{
  byte x = EEPROM.read(0);
  Serial.println(String(x));
  if (x != ":") {
    return false;
  }
  
}
