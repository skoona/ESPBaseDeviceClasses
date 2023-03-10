/**
 * @file SknDHT.hpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief  C++ Node DHT Temperature and Humidity  Sensor
 * @version 0.1
 * @date 2023-03-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
 
#pragma once

#include <DHTesp.h>
#include "Streaming.h"

// #ifdef ESP32
// #pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY !)
// #error Select ESP8266 board.
// #endif


class SknDHT {

public:

  SknDHT(const uint8_t dhtPin, DHTesp::DHT_MODEL_t dhtModel = DHTesp::DHT_MODEL_t::DHT22);

  float  getTemperature() const { return DHTesp::toFahrenheit( _sensorResults.temperature ); }
  float  getHumidity() const { return _sensorResults.humidity; }
  String getModelName();
  
  void begin();
  void loop();
  
private:
  // DHT Sensors address
  const char *cCaption = "DHT22 Humidity and Temperature Sensor";
  const char* cIndent  = "  ◦ ";

  // suggested rate is 1/60Hz (1m)
  unsigned long _measurementInterval = 60000UL;
  unsigned long _lastMeasurement = 0;

  uint8_t _dhtPin;
  DHTesp sensor;
  TempAndHumidity _sensorResults;
  DHTesp::DHT_ERROR_t _sensorStatus;
  DHTesp::DHT_MODEL_t _model;
};
