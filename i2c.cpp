#include "Arduino.h"
#include "i2c.h"

#include <Wire.h>

const int MIN_ADDRESS = 20;
const int MAX_ADDRESS = 24;

const int RETRY_INTERVAL = 5000;

I2c::I2c() {}

void I2c::init(void (*requestEvent)(), void (*receiveEvent)(int)) 
{
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

int I2c::getAddress() {
  return _address;
}

bool I2c::setAddress() 
{
  Wire.begin();
  int nDevices = 0;
  for (byte address = MIN_ADDRESS; address <= MAX_ADDRESS; ++address) {
    delay(100);

    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    Serial.print("Error : ");Serial.println(error);  
    if ((error != 0) && (error != 4)) {
      Wire.begin(address);
      _address = address;
      return true;
    }
  } 
  Serial.println("Didn't get one");
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
