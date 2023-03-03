/**
 * @file SknVL53L1x.hpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief  Class for VL53L1x.
 * @version 0.1
 * @date 2023-03-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <Wire.h>
#include <VL53L1X.h>
#include <Preferences.h>
#include "Streaming.h"

class SknVL53L1x  {

public:
  SknVL53L1x( unsigned int timingBudgetUS = 250000, unsigned int interMeasurementMS = 1000 );

  bool isActive() { return bActive && bVL53L1xInitialized; }
  bool isLearning() { return (bAutoLearnUp || bAutoLearnDown) && bVL53L1xInitialized; }
  bool isInitialized() { return bVL53L1xInitialized; }
  bool isReady() { return (isActive() || isLearning()); }
  const char* movementString();

  SknVL53L1x& start();
  SknVL53L1x& stop(); 
  SknVL53L1x& begin();
  void loop();

private :   

  enum eDirection {UP, DOWN,OPEN,LEARNING, EXIT_DIR};
       enum eMode {ACTIVE, AUTO_LEARN_UP, AUTO_LEARN_DOWN, REBOOT, EXIT_MODE};

     eDirection movement();
   unsigned int currentMM() { return uiDistanceValueMM; };  
   unsigned int currentPos() { return uiDistanceValuePos; };  
   const char * currentJSON() {return cCurrentJSON; }
   const char * formatJSON();
           void manageAutoLearn(long mmPos);
   unsigned int relativeDistance(bool wait=false);
/*
 * Door travel: 86.5" or 2198 mm
 * Mount point: 13"   or  330 mm
 * maximum range:        2528 mm
*/
#define MM_MIN 152
#define MM_MAX 2420
          bool limitsSave();
          bool limitsRestore();          
  unsigned int readValue(bool wait=true);
           int iLimitMin = MM_MIN;    // logical UP,   or closest to sensor
           int iLimitMax = MM_MAX;    // logical DOWN, or farest from sensor
  
  unsigned int uiDistanceValueMM  = 0;          // avg
  unsigned int uiDistanceValue    = 0;          // actual
  unsigned int uiDistanceValuePos = 0;          // transposed
  unsigned int uiTimingBudget;
  unsigned int uiInterMeasurement;

  unsigned long ulLastTrigger = 0;
  unsigned long ulTrigger = 60000; // 60 seconds after last idle/stop,   Stats are sent every 300 seconds or 300000 ms

      VL53L1X sensor;                        // Ranging Device
  Preferences prefs;                      // stored ranger limit min - max

  char cBuffer[128];
  char rangingJSON[256];
  const char *cSknRangerID  = "Ranger";   // memory key
  const char *cCurrentState = "Ready";    // current state/label
  const char *cCurrentMode  = "Ready";    // current mode/label
  const char *cCurrentJSON  = "{}";       // current positon details
  const char *cDir[EXIT_DIR] = {"UP","DOWN","OPEN", "LEARNING"};
  const char *cMode[EXIT_MODE] = {"READY","AUTO LEARN UP","AUTO LEARN DOWN","REBOOTING"};

  #define MAX_SAMPLES 5
      const int capacity = (MAX_SAMPLES);
  unsigned  int distances[MAX_SAMPLES + 2];
  unsigned long readings = 0;
  unsigned long cycleCount = 0;
  
  #define AUTO_LEARN_READINGS 10
  unsigned long autoLearnUpReadings = 0;
  unsigned long autoLearnDownReadings = 0;
           bool bAutoLearnUp;
           bool bAutoLearnDown;
           bool bActive = false;
           bool bVL53L1xInitialized;

  VL53L1X::RangingData sDat, *PSDat;
  // {
  //   uint16_t range_mm;
  //   RangeStatus range_status;
  //   float peak_signal_count_rate_MCPS;
  //   float ambient_count_rate_MCPS;
  // };

  const char *cCaption   = "• Garage Door Automaton Module:";
  const char *cIndent    = " ✖  ";
};
