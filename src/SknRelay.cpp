/**
 * @file SknRelay.cpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief  Class to operate Relays
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "SknRelay.hpp"

extern volatile bool gbSensorsEnabled;

SknRelay::SknRelay(const int relayPin, const int relayHold, const int relayOnLevel)
    : _relayPin(relayPin),
      _relayHold(relayHold),
      _relayOnLevel(relayOnLevel)
{
  if ((_relayHold > VALIDATION_MAX_MS) && (_relayHold < VALIDATION_MIN_MS)) {
    _relayHold = VALIDATION_DEFAULT_MS;
  }
  _relayOffLevel = !_relayOnLevel;
}

/**
 *
 */
void SknRelay::setHoldTimeInMilliseconds(const int ms)
{
  if (ms > VALIDATION_MIN_MS && ms < VALIDATION_MAX_MS)
  {
    _relayHold = ms;
    Serial << cIndent << "setHoldTimeInMilliseconds() set: " << ms << endl;
  } else {
    _relayHold = VALIDATION_DEFAULT_MS;
  }
}

/**
 *
 */
void SknRelay::toogle(bool shortCycle)
{
  if (vbEnabled) {
    int delayTime = _relayHold;

    if (shortCycle) {
      delayTime = _relayHold / 2; // half time
    }

    Serial << cIndent << "[Start] Operating Relay pin: " << _relayPin << " level: " << _relayOnLevel << " Hold: " << delayTime << " Fast: " << shortCycle << endl;
    digitalWrite(_relayPin, _relayOnLevel); // activate door relay

    // delay(delayTime); // Delay is not working in this class ???
    vTaskDelay(delayTime/portTICK_PERIOD_MS);
    // while (millis() < time_now + delayTime) {}

    digitalWrite(_relayPin, _relayOffLevel); // de-activate door relay
    Serial << cIndent << "[Stop ] Operating Relay" << endl;
  }
}

/**
 *
 */
void SknRelay::loop() {  
  taskYIELD();
}

/**
 *
 */
void SknRelay::begin() {
  pinMode(_relayPin, OUTPUT); // Door operator
  digitalWrite(_relayPin, _relayOffLevel); // Init door to off

  vbEnabled = false;
}
