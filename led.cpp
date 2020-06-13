#include "Arduino.h"
#include "led.h"

const long BLINK_INTERVAL = 333;

Led::Led()
{
}

void Led::init(int pin_r, int pin_g, int pin_b){
  _pin_r = pin_r;
  _pin_g = pin_g;
  _pin_b = pin_b;
  pinMode(_pin_r, OUTPUT);
  pinMode(_pin_g, OUTPUT);
  pinMode(_pin_b, OUTPUT);
  _led_on = true;
  _timer = millis();
}


void Led::setColour(int red, int green, int blue)
{
//  #ifdef COMMON_ANODE
//    red = 255 - red;
//    green = 255 - green;
//    blue = 255 - blue;
//  #endif
  analogWrite(_pin_r, red);
  analogWrite(_pin_g, green);
  analogWrite(_pin_b, blue);   
}

void Led::blinkColour(int red, int green, int blue) {
  if (((int) millis() - _timer) > BLINK_INTERVAL) {
    if (_led_on) {
      analogWrite(_pin_r, 0);
      analogWrite(_pin_g, 0);
      analogWrite(_pin_b, 0); 
      _led_on = false;
      _timer = millis();
    }
    else {
      analogWrite(_pin_r, red);
      analogWrite(_pin_g, green);
      analogWrite(_pin_b, blue); 
      _led_on = true;
      _timer = millis();
    }
  }
}
