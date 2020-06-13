#include "Arduino.h"
#include "i2c.h"

#include <Wire.h>

const int MIN_ADDRESS = 10;
const int MAX_ADDRESS = 12;

const int RETRY_INTERVAL = 5000;

I2c::I2c() {}

void I2c::init(void (*requestEvent)(), void (*receiveEvent)(int)) 
{
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

bool I2c::setAddress() 
{
  if (((int) millis() - _timer) > RETRY_INTERVAL) {
    Wire.begin();
    int nDevices = 0;
    for (byte address = MIN_ADDRESS; address <= MAX_ADDRESS; ++address) {
      
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      byte error = Wire.endTransmission();
      if ((error != 0) && (error != 4)) {
        Serial.println("Got one");  
        _address = address;
        Wire.begin(_address);
        
        _timer = millis();
        return true;
      }
    } 
    _timer = millis();
    Serial.println("Didn't get one");
  }
  return false;
 
}

bool I2c::checkAddressUnique() 
{
  Wire.begin();
  Wire.beginTransmission(_address);
  byte error = Wire.endTransmission();
  if ((error != 0) && (error != 4)) { 
    Serial.println("Address OK!");  
    Wire.begin(_address);
    return true;
  }
  else {
    Serial.println("Address conflict!");
    return false;
  }
}
