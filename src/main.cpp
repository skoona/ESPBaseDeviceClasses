/**
 * @file main.cpp
 * @author James Scott, Jr.  aka(Skoona) (skoona@gmail.com)
 * @brief ESP Base Classes 
 * @version 1.0.0
 * @date 2021-01-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <Arduino.h>
#include "SknDHT.hpp"
#include "SknRelay.hpp"
#include "SknLD2410.hpp"
#include "SknVL53L1x.hpp"


#ifdef ESP8266
extern "C"
{
#include "user_interface.h" // to set CPU Freq for Non-Huzzah's
}
#endif

// #undef LED_BUILTIN
// #define LED_BUILTIN 4
// LONLIN32 uses pin 5 for LED

// Select pins for Temperature and Motion
// esp32doit-devkit-v1  pins
#define SDA         21
#define SCL         22

#define RELAY_PIN    5    // sonoff

#define DHT_PIN      4    // D4  4
#define DHT_TYPE DHTesp::DHT_MODEL_t::DHT11 // DHTesp::DHT22

#define LD_IO       18    // D18        
#define LD_RX       16    // D6
#define LD_TX       17    // D7
#define LD2410_TARGET_REPORTING false   // enables status property

#define LOX_TIMING_BUDGET_US    250000        // = 250 ms
#define LOX_INTERMEASUREMENT_MS   1000        // = 1000 ms
#define VALIDATION_DEFAULT_MS      400 

// guard-flag to prevent sending properties when offline
volatile bool gbSensorsEnabled=false;

SknLD2410    presence(LD_RX, LD_TX, LD_IO);
SknDHT    environment(DHT_PIN, DHT_TYPE);
SknVL53L1x        lox(LOX_TIMING_BUDGET_US, LOX_INTERMEASUREMENT_MS);
SknRelay        relay(RELAY_PIN, VALIDATION_DEFAULT_MS, HIGH);

void setup() {
  delay(250);
  Serial.begin(115200);
  delay(250);  
  Serial << "Starting..." << endl;

  environment.begin();
  relay.begin();
  lox.begin();
  presence.begin();

  // enable all classes
  gbSensorsEnabled = true;
}

unsigned long lastInterval = millis();
void loop() {
  environment.loop();
  relay.loop();
  if((millis()-lastInterval) >= 15000) {
    lastInterval = millis();
    relay.toogle();
  }
  lox.loop();
  presence.loop();
}