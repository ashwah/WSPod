#ifndef Eeprom_h
#define Eeprom_h

#include "Arduino.h"
#include <EEPROM.h>

class Eeprom
{
  public:
    Eeprom();
    String getUuid();
    bool hasUuid();
    void setUuid(String uuid);

  private:
//    unsigned int _timer = 0;
//    int _pin_r;
//    int _pin_g;
//    int _pin_b;
};

#endif
