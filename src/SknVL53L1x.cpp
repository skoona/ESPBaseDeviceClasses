/**
 * @file SknVL53L1x.cpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief Class for VL53L1x.
 * @version 0.1
 * @date 2023-03-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "SknVL53L1x.hpp"

extern volatile bool gbSensorsEnabled;


SknVL53L1x::SknVL53L1x( unsigned int timingBudgetUS, unsigned int interMeasurementMS )    
  : uiTimingBudget(timingBudgetUS),
    uiInterMeasurement(interMeasurementMS),
    sensor()
{
  if(timingBudgetUS<1000) {
    uiTimingBudget=timingBudgetUS*1000; 
  }

  /*
   * Initialze averaging array */
  for (int idx = 0; idx < (MAX_SAMPLES); idx++) {
      distances[idx] = 0;
  }
}

/**
 * VL53L1x Device Init
 * - delay(1000); Delay is not working in this class ???
 * - one reading every second
 */
SknVL53L1x& SknVL53L1x::begin( ) {
  Serial << cCaption << endl;
  Wire.begin();
  if(isInitialized()) return *this;     // no double dips

  limitsRestore(); // load the limits

  Serial.printf("✖  SknVL53L1x initialization starting.\n");
  sensor.setTimeout(uiInterMeasurement+(2 * (uiTimingBudget/1000)));

  while (!sensor.init()) {
    Serial.printf("✖  Failed to detect and initialize sensor!\n");
    delay(1000);
  }
  Serial.printf(" 〽  Exited initialize sensor!\n");

  sensor.setTimeout(uiInterMeasurement+(2 * (uiTimingBudget/1000)));

  if (sensor.setDistanceMode(VL53L1X::Long)) {  
    Serial.printf(" 〽 Medium distance mode accepted.\n");
  }

  if (sensor.setMeasurementTimingBudget(uiTimingBudget)) {
    Serial.printf(" 〽 %ums timing budget accepted.\n", uiTimingBudget/1000);
  } 
  Serial.printf("✖  SknVL53L1x initialization Complete.\n");
  
  bVL53L1xInitialized=true;
  
  cCurrentState=cDir[OPEN];
  cCurrentState=cMode[ACTIVE];

  return(*this);
}

/**
 * @brief Start device
 * 
 *  250ms read insode 1000ms wait = cycle time of 1000ms
*/
SknVL53L1x&  SknVL53L1x::start() {
  if(!isInitialized()) return *this;
  if(!isActive()) {
    sensor.startContinuous(uiInterMeasurement + (2 * (uiTimingBudget/1000)));
    bActive=true;
    cycleCount = readings + 60;    
    Serial.printf("✖  SknVL53L1x startContinuous(%ums) accepted.\n", uiInterMeasurement + (2 * (uiTimingBudget/1000)));
  }
  return *this;
}

/**
 * Stop device
*/
SknVL53L1x& SknVL53L1x::stop() {
  if(isActive()) {    
    sensor.stopContinuous();  
    bActive=false;
    ulLastTrigger = millis();
    Serial.printf("✖  SknVL53L1x stopContinuous() accepted.\n");
    // broadcastStatus();
  }
  return *this;
}

/**
 * determine direction of movement
*/
SknVL53L1x::eDirection SknVL53L1x::movement() {
  eDirection eDir = OPEN;

  if ( uiDistanceValuePos==0) { eDir=UP; }
  if ( uiDistanceValuePos==100) { eDir=DOWN; }
  
  if(bAutoLearnUp) { eDir=LEARNING; } 
  if(bAutoLearnDown) { eDir=LEARNING; }
  
  return eDir;
}

/**
 * @brief movement converted to string
 * 
 */
const char* SknVL53L1x::movementString() {
  return cDir[ movement() ];
}

void SknVL53L1x::manageAutoLearn(long mmPos) {
    /*
   * determine the new ranges and save to eeprom */
  readings++;
  if(bAutoLearnUp) {
    iLimitMin = mmPos;
    cCurrentState=cDir[LEARNING];
    cCurrentMode = cMode[AUTO_LEARN_UP];
    if(readings>=autoLearnUpReadings) {
      bAutoLearnUp=false;
      limitsSave();
      snprintf(cBuffer, sizeof(cBuffer), "Auto Learn Range, Up %d mm, Down %d mm", iLimitMin, iLimitMax);
      cCurrentMode = cBuffer;
      cCurrentState=movementString();
      stop();
      Serial.printf("✖  SknVL53L1x Auto Learn Up(%d mm) accepted.\n", iLimitMin);
    }
  } else if (bAutoLearnDown) {
    iLimitMax = mmPos;
    cCurrentState=cDir[LEARNING];
    cCurrentMode = cMode[AUTO_LEARN_DOWN];
    if(readings>=autoLearnDownReadings) {
      bAutoLearnDown=false;
      limitsSave();
      snprintf(cBuffer, sizeof(cBuffer), "Auto Learn Range, Up %d mm, Down %d mm", iLimitMin, iLimitMax);
      cCurrentMode = cBuffer;
      cCurrentState=movementString();
      stop();
      Serial.printf("✖  SknVL53L1x Auto Learn Down(%d mm) accepted.\n", iLimitMax);
    }
  }

}

/**
 * @brief JSON formatted Position
 * 
 */
const char * SknVL53L1x::formatJSON() {
  snprintf(rangingJSON, sizeof(rangingJSON), "{\"range\":%u,\"average\":%u,\"mapped\":%u,\"status\":\"%s\",\"raw_status\":%u,\"signal\":%3.1f,\"ambient\":%3.1f,\"movement\":\"%s\"}",
                uiDistanceValue,
                uiDistanceValueMM,
                uiDistanceValuePos,
                sensor.rangeStatusToString(sDat.range_status),
                sDat.range_status,
                sDat.peak_signal_count_rate_MCPS,
                sDat.ambient_count_rate_MCPS,
                movementString()); 
  cCurrentJSON = rangingJSON;
  return rangingJSON;
}

/**
 * @brief read value when available ready and return average value.
 * 
 * - array was initialized in class init.
 * - affect uiDistanceValueMM if valid, return avgerage
 * - copied result struct if valid
 * - return 0 value if invalid
*/
unsigned int SknVL53L1x::readValue(bool wait)
{
  unsigned long sum = 0;
  unsigned long avg = 0;
  unsigned int value = 0;

  value = (unsigned int)sensor.readRangeContinuousMillimeters(wait);
  
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  if (sensor.ranging_data.range_status == VL53L1X::RangeValid)  {
    sDat.range_mm = sensor.ranging_data.range_mm;
    sDat.range_status = sensor.ranging_data.range_status;
    sDat.peak_signal_count_rate_MCPS = sensor.ranging_data.peak_signal_count_rate_MCPS;
    sDat.ambient_count_rate_MCPS = sensor.ranging_data.ambient_count_rate_MCPS;

    for (int idx = 0; idx < capacity; idx++)
    {
      distances[idx] = distances[idx + 1]; // move all down
      sum += distances[idx];
    }

    distances[capacity] = value;
    avg = (sum / capacity);
    uiDistanceValueMM = (unsigned int)avg;
    uiDistanceValue = value;
  }  else   {
    distances[capacity] = value;
    avg = 0;
  }

  Serial.printf(" 〽 [%6.0lu] range: %u mm  avg: %lu mm\tstatus: %s\traw status: %u\tsignal: %3.1f MCPS\tambient: %3.1f MCPS\tmove: %s\n",
                    readings,
                    value,
                    avg,
                    sensor.rangeStatusToString(sensor.ranging_data.range_status),
                    sensor.ranging_data.range_status,
                    sensor.ranging_data.peak_signal_count_rate_MCPS,
                    sensor.ranging_data.ambient_count_rate_MCPS,
                    movementString()); 
  
  return avg;
}

/**
 * Transform mm to relative range
 */
unsigned int SknVL53L1x::relativeDistance(bool wait) {
  long posValue;
  long mmPos = 0;
  
  /*
   * get valid value */
  do {
    
    mmPos = (long)readValue(wait);

    if (mmPos==0) { Serial.printf(" 〽  relativeDistance(0 mm) NOT accepted.\n"); }

  } while(mmPos==0); // get a vaild value before proceeding

  /*
   * determine the new ranges and save to eeprom */
  manageAutoLearn(mmPos);

  /*
   * use fixed ranges while determining new range */
  if(isLearning()) {
    posValue = map(mmPos, MM_MIN, MM_MAX, 0, 100);
  } else {
    posValue = map(mmPos, iLimitMin, iLimitMax, 0, 100);
    if(readings>cycleCount) {
      stop(); 
    } 
  }

  mmPos = constrain( posValue, 0, 100);
  if(mmPos!=uiDistanceValuePos) {
    uiDistanceValuePos = mmPos;
    formatJSON();
    // setProperty(cSknPosID).send(String(mmPos));
    // setProperty(cSknDetailID).send(cCurrentJSON);
  }
  formatJSON();

  Serial.printf("✖  relativeDistance(%ld %%) accepted.\n", mmPos);

  return (unsigned int) mmPos;
}

/*
  * Save position limits */
bool SknVL53L1x::limitsSave() {
  bool rc = false;
  prefs.begin(cSknRangerID, false);
      prefs.putInt("mm_min", iLimitMin);
      prefs.putInt("mm_max", iLimitMax);
      rc = prefs.isKey("mm_min");
  prefs.end();
  
  Serial.printf(" 〽  limitsSave(%s) \tmin: %d \tmax: %d\n", (rc ? "True": "False"), iLimitMin, iLimitMax);
  return rc;
}

 /*
  * Restore position limits */
bool SknVL53L1x::limitsRestore() {
  bool rc = false;
  prefs.begin(cSknRangerID, false);
  if (prefs.isKey("mm_min")) {
    iLimitMin = prefs.getInt("mm_min", MM_MIN);
    iLimitMax = prefs.getInt("mm_max", MM_MAX);
    rc = true;
  }
  prefs.end();

  Serial.printf(" 〽  limitsRestore(%s) \tmin: %d \tmax: %d\n", (rc ? "True": "False"), iLimitMin, iLimitMax);
  return rc;
}

/**
 * Handles the received MQTT messages from Homie.
 *

bool SknVL53L1x::handleInput(const HomieRange& range, const String& property, const String& value) {
  bool rc = false;
  Serial << cIndent << "handleInput -> property '" << property << "' value=" << value << endl;

    // Node Services
  if(property.equalsIgnoreCase(cSknModeID)) {
    if(value.equalsIgnoreCase("reboot")) {
      stop();
      Serial << cIndent << "RESTARTING OR REBOOTING MACHINE ";
      cCurrentMode =  cMode[REBOOT];
      cCurrentState = cDir[OPEN];
      ESP.restart();
      rc = true;
    } else if (value.equalsIgnoreCase("auto_learn_up")) {
      Serial << cIndent << "Auto Learn Up ";
      cCurrentMode =  cMode[AUTO_LEARN_UP];
      cCurrentState = cDir[LEARNING];
      autoLearnUpReadings = readings + AUTO_LEARN_READINGS;
      bAutoLearnUp = true;
      if(!bActive) start();      
      rc = true;
    } else if (value.equalsIgnoreCase("auto_learn_down")) {
      Serial << cIndent << "Auto Learn Down ";
      cCurrentMode =  cMode[AUTO_LEARN_DOWN];
      cCurrentState = cDir[LEARNING];
      autoLearnDownReadings = readings + AUTO_LEARN_READINGS;
      bAutoLearnDown = true;
      if(!bActive) start();      
      rc = true;
    }     
  }
  return rc;
}
*/


/**
 *
*/
/*
void SknVL53L1x::broadcastStatus() {
  if(gbEnableDoorOperations) {
    cCurrentState=movementString();
    setProperty(cSknPosID).send(String(uiDistanceValuePos));
    setProperty(cSknStateID).send(cCurrentState);
    setProperty(cSknModeID).send(cCurrentMode);
    setProperty(cSknDetailID).send(cCurrentJSON);
  }
}
*/


/**
 * Processing loop
 * - start after every 60 seconds after last idle period
 * - process reading once mqtt is active 
 *   and vl53l1x has been initialized.
 */
void SknVL53L1x::loop() {
  if(!isActive()) {
    if( millis() > (ulLastTrigger + ulTrigger) ) {     
      start();
    }
  }

  if(gbSensorsEnabled) {
    if( isReady() ) {
      relativeDistance(true);   
    }
  }
}

