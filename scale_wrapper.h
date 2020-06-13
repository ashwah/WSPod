#ifndef ScaleWrapper_h
#define ScaleWrapper_h

#include "Arduino.h"
#include <HX711.h>

class ScaleWrapper
{
  public:
    ScaleWrapper();
    void init(int pin_dout, int pin_clk);
    void tare();
    bool isReady();
    bool isStable();
    bool isChanging();
    bool isError();
    bool hasNewWeight();
    float getCurrentWeight();

  private:
    HX711 _scale;
    int _pin_dout;
    int _pin_clk;
    int _state;
    bool _waiting = false;
    int _step = 0;
    unsigned int _timer;
    float _display_weight;
    float _suggest_weight;
    float _current_weight;
    bool _error;
    bool significantChange(float w1, float w2);
};

#endif
