/**
 * @file SknLD2410.cpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief  LD2410 mmWave sensor Motion Sensor
 * @version 0.1
 * @date 2023-03-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "SknLD2410.hpp"

extern volatile bool gbSensorsEnabled;

SknLD2410::SknLD2410(const uint8_t rxPin, const uint8_t txPin, const uint8_t ioPin)
  : _rxPin(rxPin),
    _txPin(txPin),
    _ioPin(ioPin),
    sensor()
{
  // Start up the library
  pinMode(_ioPin, INPUT);
}

bool SknLD2410::hasMotion()    { return sensor.isMoving(); }
bool SknLD2410::hasOccupied()  { return sensor.isStationary(); }
bool SknLD2410::hasPresence()  { return (sensor.isMoving() || sensor.isStationary()); }

/**
 * Called by Homie when Homie.setup() is called; Once!
*/
void SknLD2410::begin() {
  Serial << cCaption << endl;

  // start path to LD2410
  // sensor.debug(Serial);  // enable debug output to console
  Serial2.begin(256000, SERIAL_8N1, _rxPin, _txPin); // UART for monitoring the sensor rx, tx  

  if (sensor.begin(Serial2)) {
    Serial << cCaption << " Initialized..." << endl;
  } else {
    Serial << cCaption << " was not connected" << endl;
  }

}

/**
 * Is connected and in run mode
*/
void SknLD2410::loop() {
  _timer = millis();

  sensor.ld2410_loop();
  
  // Module controls hold time, so all we need is to read the io pin
  if ((_motion != digitalRead(_ioPin)) || ((_timer - _lastBroadcast) > _broadcastInterval)) {       
    _lastBroadcast = _timer;
    _motion = digitalRead(_ioPin);
  }
}

/*
 * Translate current status
*/
const char * SknLD2410::triggeredby() {
  const char * triggered = "";

  if(sensor.isMoving() && sensor.isStationary()) {
    triggered = "Occupied and Moving";
  } else if (sensor.isStationary()) {
    triggered = "Occupied";
  } else if(sensor.isMoving()) {
    triggered = "Moving";
  } else {
    triggered = "No Presence";
  }

  return triggered;
}

