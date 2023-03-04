/**
 * @file SknLD2410.hpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief LD2410 mmWave sensor Motion Sensor
 * @version 0.1
 * @date 2023-03-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <ld2410.h>
#include "Streaming.h"

class SknLD2410 {

public:
  SknLD2410(const uint8_t rxPin, const uint8_t txPin, const uint8_t ioPin);
 
  bool hasMotion();
  bool hasOccupied();
  bool hasPresence();

  void begin();
  void loop();
  const char * triggeredby();

private:
  const char *cCaption = "[SknLD2410] • mmWave sensor Motion Sensor";
  const char *cIndent = " ✖  ";

  // Motion Node Properties
  const uint8_t _rxPin;
  const uint8_t _txPin;
  const uint8_t _ioPin;

  // Loop Interval Parms
  uint32_t _broadcastInterval = 15000;
  uint32_t _lastBroadcast = 0;
  uint32_t _timer = 0;

  // LD2410 Interface
  volatile bool _motion = false;
  ld2410 sensor;

};
