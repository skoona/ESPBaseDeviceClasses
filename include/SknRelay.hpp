/**
 * @file SknRelay.hpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief  Class to operate Relays
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
 
#pragma once

#include <Arduino.h>
#include "Streaming.h"

#define VALIDATION_MAX_MS 1000
#define VALIDATION_MIN_MS 200
#define VALIDATION_DEFAULT_MS 400 

class SknRelay {

public:
  SknRelay(const int relayPin, const int relayHold, const int relayOnLevel = HIGH);

  void toogle(bool shortCycle=false);
  void setHoldTimeInMilliseconds(const int ms);
  void begin();
  void loop();

private:
  int _relayPin;
  int _relayHold;
  const int _relayOnLevel = HIGH;
  int _relayOffLevel = !_relayOnLevel;

  const char *cCaption = "• Relay Module:";
  const char *cIndent = " ✖  ";

  const char *cRelayID = "operate";

  volatile bool vbEnabled = false;                 // operating trigger
};
