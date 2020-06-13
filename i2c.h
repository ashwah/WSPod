#ifndef I2c_h
#define I2c_h

#include "Arduino.h"
#include <Wire.h>


class I2c
{
  public:
    I2c();
    void init(void (*requestEvent)(), void (*receiveEvent)(int));
    bool setAddress(); 
    bool checkAddressUnique();

  private:
    unsigned int _timer = -30000;
    int _address;
};

#endif
