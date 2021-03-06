#include "Arduino.h"

#include "ws_state.h"
#include "led.h"
#include "scale_wrapper.h"
#include "eeprom.h"
#include "i2c.h"

// RGB must be pins that function as analog.
const int RRR = 9;
const int GGG = 10;
const int BBB = 11;

// Define pins for scales.
const int CLK = 8;
const int DOUT = 7;

int currentMillis;

WsState::WsState()
{

}

void WsState::init(void (*requestEvent)(), void (*receiveEvent)(int)) {
  led.init(RRR, GGG, BBB);
  scaleWrapper.init(DOUT, CLK);
  i2c.init(requestEvent, receiveEvent);
  _state = INIT;
  _timer = millis();

}

void WsState::runState()
{
  switch (_state)
  {
    case INIT:
      led.setColour(255, 0, 0);  // red
      this->wait(750, WAIT_SCALE);
      Serial.println("WeightCore initalizing");
      break;

    case WAIT_SCALE:
      led.blinkColour(194, 32, 0);  // yellow
      Serial.println("WeightCore checking scale");
      if (scaleWrapper.isReady()) {
        scaleWrapper.tare();
        _state = INIT_I2C;
      }
      break;

    case INIT_I2C:
      led.setColour(0, 255, 255);  // blue+green
      if (i2c.setAddress()) {
        _state = WAIT_I2C;
      }
      else {
        _state = FAIL;
      }
      Serial.println("WeightCore joining I2C bus");
      break;

    case WAIT_I2C:
      // This state will be updated in the receive event interrupt.
      led.blinkColour(0, 255, 255);
      break;


    case INIT_UUID:
      led.setColour(0, 0, 255);
      Serial.println("init uuid");
      //eeprom.deleteUuid();
      if (eeprom.hasUuid()) {
        Serial.print("has uuid : ");        
        _uuid = eeprom.getUuid();
        Serial.print(_uuid);
        _state = STABLE;
      }
      else {
        Serial.println("no uuid");
        _state = WAIT_UUID;
      }

    case WAIT_UUID:
      // This state will be updated in the receive event interrupt.
      led.setColour(0, 0, 255);  // blue
      break;

    case STABLE:
      led.setColour(0, 255, 0);  // green
      if (!scaleWrapper.isStable()) {
        if (scaleWrapper.isError()) {
          _state = ERROR_STATE;
        }
        else {
          _state = CHANGING;
        }
      }
      break;

    case CHANGING:
      led.blinkColour(0, 255, 0);  // green
      if (!scaleWrapper.isChanging()) {
        if (scaleWrapper.isError()) {
          _state = ERROR_STATE;
        }
        else {
          if (scaleWrapper.hasNewWeight()) {
            _state = SENDING_CHECK;
          }
          else {
            _state = STABLE;
          }
        }
      }
      break;

    case SENDING_CHECK:
      // Check i2c address is definitely unique.
      if (!i2c.checkAddressUnique()) {
        if (i2c.setAddress()) {
          _state = SENDING;
        }
        else {
          _state = FAIL;
        }

      }
      else {
        _state = SENDING;
      }
      break;

    case SENDING:
      led.setColour(255, 0, 255); // Purple
      // TODO temp
      //this->wait(2000, STABLE);
      // This state is exited via the receive event interrupt.
      break;

    case SENDING_ACKNOWLEDGED:
      led.setColour(255, 0, 255); // Purple

      this->wait(2000, STABLE);
      break;

    case ERROR_STATE:
      Serial.println("Error :(");
      led.blinkColour(255, 0, 0); // Red
      if (scaleWrapper.isReady()) {
        _state = STABLE;
      }
      break;

    case FAIL:
      Serial.println("Death :(");
      led.setColour(255, 0, 0); // Red
      break;
  }
}

/*
   Provides a non-blocking wait method.
*/
void WsState::wait(int wait_time, uint8_t transition_state) {
  if (!_waiting) {
    _waiting = true;
    _timer = millis();
  }
  else {
    if (((int) (millis() - _timer)) > wait_time) {
      _state = transition_state;
      _waiting = false;
      _timer = millis();
    }
  }
}


/**
   For sending data down the i2c wire it was necessary to override the buffer.

   The following constants were changed:
   TWI_BUFFER_LENGTH 64 in
   /home/ashley/Downloads/arduino-1.8.12-linux64/arduino-1.8.12/hardware/arduino/avr/libraries/Wire/src/utility/twi.h

   BUFFER_LENGTH 64 in
   /home/ashley/Downloads/arduino-1.8.12-linux64/arduino-1.8.12/hardware/arduino/avr/libraries/Wire/src/wire.h
*/
static void WsState::requestEvent() {
  if (_state == SENDING) {
    // Add the code.
    Wire.write("1");

    // Add the current weight.
    float w = scaleWrapper.getCurrentWeight();
    char buf[5];
    String(w).toCharArray(buf, 6);
    if (w >= 0) {
      Wire.write(" ");
    }
    Wire.write(buf);

    // Add the uuid.
    char buf2[37];
    _uuid.toCharArray(buf2,37);
    Wire.write(buf2);
  }
  else if (_state == WAIT_I2C) {
    Wire.write("2");
  }
  else if (_state == WAIT_UUID) {
    Wire.write("3");
  }
  else {
    Wire.write("0");
  }
}

/*
   Handles all the incoming data from the hub.

   The first byte defines the data code:
   1 -
*/
void WsState::receiveEvent(int dataLength) {
  
  char code;
  while (Wire.available()) // loop through all but the last
  {
    code = Wire.read(); // receive byte as a character
//    Serial.print("Data code : ");
//    Serial.println(code);
    
    // 1 - Weight data acknowledged.
    if (code == '1' && _state == SENDING) {
      Serial.println("Sending Acknowledged");
      _state = SENDING_ACKNOWLEDGED;
    }

    // 2 - I2C Handshake.
    if (code == '2' && _state == WAIT_I2C) {
      Serial.println("I2C Acknowledged");
      _state = INIT_UUID;
    }

    // 3 - UUID from API.
    if (code == '3' && _state == WAIT_UUID) {
      // TODO set the uuid from the wire.
      Serial.print("In UUID event : ");

      String uuid = "";
      char c;
      int i = 0;
      // Get the actual UUID.
      while (Wire.available() && i < 37) {
        c = Wire.read();
        uuid += c;
        i++;
      }
      Serial.println(uuid);
      eeprom.setUuid(uuid);
      _uuid = uuid;
      _state = STABLE;
    }
  }
}
