#include "Arduino.h"
#include <HX711.h>
#include "scale_wrapper.h"

HX711 _scale;

const int READING_INTERVAL = 1000;

// Define calibration parameters.
const float THRESHOLD_DIFFERENCE = 0.15;
const float THRESHOLD_PERCENT = 0.05;
const int CHANGE_STEPS = 5;
const int CALIBRATION_FACTOR = -10550.0;

ScaleWrapper::ScaleWrapper()
{
}

void ScaleWrapper::init(int pin_dout, int pin_clk)
{
  _pin_dout = pin_dout;
  _pin_clk  = pin_clk;
 
  _scale.set_scale(CALIBRATION_FACTOR);
  _scale.begin(_pin_dout, _pin_clk);
}

void ScaleWrapper::tare() 
{
  _scale.tare();
}

bool ScaleWrapper::isReady()
{
  if (_scale.is_ready() == true) {
    return true;
  }
  else {
    return false;
  }
}

bool ScaleWrapper::isStable()
{
  _error = false;
  if (((int) millis() - _timer) > READING_INTERVAL) {
    Serial.print("Timer: ");Serial.println(_timer);

    _timer = millis();
    if (_scale.is_ready()) {
      _current_weight = _scale.get_units();
      Serial.print("Stable reading: ");Serial.println(_current_weight);
    } 
    else {
      Serial.println("HX711 not found.");
      _error = true;
      return false;
    }
    if (this->significantChange(_current_weight, _display_weight)) {
      _suggest_weight = _current_weight;
      return false;
    }      
  }

  return true;
}

bool ScaleWrapper::isChanging()
{
  _error = false;
  if (((int) millis() - _timer) > READING_INTERVAL) {   
    Serial.print("Timer: ");Serial.println(_timer);

    _timer = millis();
    if (_scale.is_ready()) {
      _current_weight = _scale.get_units();
      Serial.print("Changing reading: ");Serial.println(_current_weight);
    } 
    else {
      Serial.println("HX711 not found.");
      _error = true;
      return false;
    }
    
    // If there's a significant deviation from our suggestion, update the suggestion
    // to the current weight and start counting again.
    if (this->significantChange(_current_weight, _suggest_weight)) {
      _suggest_weight = _current_weight;
      _step = 0;
      return true;
    }
    _step++;
    Serial.print("Step: ");Serial.println(_step);
    if (_step > CHANGE_STEPS) {
      _step = 0;
      return false;
    }
  }

  return true;
}

bool ScaleWrapper::hasNewWeight() 
{
  if (this->significantChange(_current_weight, _display_weight)) {
    _display_weight = _suggest_weight;
    return true;
  }
  else {
    _display_weight = _suggest_weight;
    return false;
  }
  
}

bool ScaleWrapper::isError() 
{
  if (_error == true) {
    return true;
  }
  else {
    return false;
  }
}

bool ScaleWrapper::significantChange(float w1, float w2)
{
  float difference;
  float percent_change;

  difference = fabs(w1 - w2);

  // If the original value was small, don't consider percentage change. Just check the absolute value difference.
  if (fabs(w1) < THRESHOLD_DIFFERENCE) {
    return difference > THRESHOLD_DIFFERENCE;
  }

  // But in other cases check the percentage change too.
  percent_change = fabs(difference / w1);
  return difference > THRESHOLD_DIFFERENCE && percent_change > THRESHOLD_PERCENT;
}

float ScaleWrapper::getCurrentWeight() 
{
  return _current_weight;
}
