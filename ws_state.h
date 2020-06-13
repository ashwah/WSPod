#ifndef WsState_h
#define WsState_h

#include "Arduino.h"
#include "led.h"
#include "scale_wrapper.h"
#include "eeprom.h"
#include "i2c.h"

class WsState
{
  public:
    WsState();
    void init(void (*requestEvent)(), void (*receiveEvent)(int));
    void runState();
    void wait(int wait_time, uint8_t transition_state);
    void requestEvent();
    void receiveEvent(int howMany);
    Led led;
    ScaleWrapper scaleWrapper;
    Eeprom eeprom;
    I2c i2c;

  private:
    enum State_enum {INIT, WAIT_SCALE, INIT_I2C, WAIT_I2C, WAIT_UUID, STABLE, CHANGING, SENDING_CHECK, SENDING, SENDING_ACKNOWLEDGED, ERROR_STATE, FAIL};
    uint8_t _state;
    String _uuid;
    unsigned long _timer;
    bool _waiting = false;
};

#endif
