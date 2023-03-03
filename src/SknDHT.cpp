/**
 * @file SknDHT.cpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief  C++ Node for DHT Temperature and Humidity  Sensor
 * @version 0.1
 * @date 2023-03-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */
 
#include "SknDHT.hpp"

extern volatile bool gbSensorsEnabled;

SknDHT::SknDHT(const uint8_t dhtPin, DHTesp::DHT_MODEL_t dhtModel)
  : _dhtPin(dhtPin),
    _model(dhtModel),
    sensor()
{}


String SknDHT::getModelName() {
  String res;

  switch (sensor.getModel()) {
    case DHTesp::DHT_MODEL_t::DHT11:
      res = "DHT11";
      break;
    case DHTesp::DHT_MODEL_t::DHT22:
      res = "DHT22";
      break;
    case DHTesp::DHT_MODEL_t::AM2302:
      res = "AM2302";
      break;
    case DHTesp::DHT_MODEL_t::RHT03:
      res = "RHT03";
      break;
    case DHTesp::DHT_MODEL_t::AUTO_DETECT:
      res = "AUTO_DETECT";
      break;
    default:
        break;
  }
  return res;
}

/**
 * called Once!
*/
void SknDHT::begin() {
  Serial << cCaption << endl;

  sensor.setup(_dhtPin, _model);

  Serial << cIndent 
                    << F("Sensor Model:  ") 
                    << getModelName() 
                    << endl;
  Serial << cIndent 
                    << F("Sensor Status: ") 
                    << sensor.getStatusString() 
                    << endl;
}

/**
 * Is connected and in run mode
*/
void SknDHT::loop() {
  if (!(millis() - _lastMeasurement >= _measurementInterval * 1000UL || _lastMeasurement == 0)) {
    return;
  }
  _lastMeasurement = millis();
  
  _sensorResults = sensor.getTempAndHumidity();
  _sensorStatus  = sensor.getStatus();

  if (_sensorStatus == DHTesp::ERROR_NONE)
  {
    Serial << cIndent
          << F("Temperature=")
          << getTemperature()
          << F(", Humidity=")
          << getHumidity()
          << endl;
    // setProperty(cTemperature).send(String( getTemperatureF() ));
    // setProperty(cHumidity).send(String( getHumidity() ));
  }
  else
  {
    Serial << cIndent
          << F("✖ Error reading sensor: ")
          << sensor.getStatusString()
          << ", value (F) read=" << _sensorResults.temperature
          << endl;
  }
}

