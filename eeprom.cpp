#include "Arduino.h"
#include <EEPROM.h>
#include "eeprom.h"

const int UUID_LENGTH = 37; // This includes a leading ":" character.

Eeprom::Eeprom()
{
}

void Eeprom::setUuid(String uuid)
{
  uuid = ":" + uuid;
  // This will eventually come from the API.
  for (int i = 0; i < UUID_LENGTH; i++)
  {
    Serial.print("Writing byte: ");    Serial.print(i);Serial.println("= " + String(uuid[i]));
    EEPROM.write(i, uuid[i]);
  }
} 

String Eeprom::getUuid()
{
  String uuid;
  for (int i = 1; i < UUID_LENGTH; i++)
  {
   uuid += (char) EEPROM.read(i);
  }
  Serial.print("From eeprom : ");
  Serial.println(uuid);
  return uuid;
}


bool Eeprom::hasUuid()
{
  char x = EEPROM.read(0);
  Serial.print("First byte : ");Serial.println(x);
  if (x != ':') {
    return false;
  }
  else {
    return true;
  }
}
