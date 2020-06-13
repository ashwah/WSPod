
#include "ws_state.h"

WsState state;

void setup() 
{ 
  Serial.begin(9600);
  state.init(requestEvent, receiveEvent);
}

void loop() 
{
  state.runState();
  delay(1);

}

void requestEvent()
{
  state.requestEvent();
}

void receiveEvent(int howMany)
{
  state.receiveEvent(howMany);
}
