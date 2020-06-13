#ifndef Led_h
#define Led_h

#include "Arduino.h"

class Led
{
  public:
    Led();
    void init(int pin_r, int pin_g, int pin_b);
    void setColour(int red, int green, int blue);
    void blinkColour(int red, int green, int blue);

  private:
    bool _led_on;
    unsigned int _timer = 0;
    int _pin_r;
    int _pin_g;
    int _pin_b;
};

#endif
