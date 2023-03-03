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

// ARDUINO_ESP32_DEV

#ifdef ESP8266
extern "C"
{
#include "user_interface.h" // to set CPU Freq for Non-Huzzah's
}
#endif

#undef LED_BUILTIN
#define LED_BUILTIN 4

// Select pins for Temperature and Motion
// esp32doit-devkit-v1  pins
#define ATM_PLAYER_DISABLE_TONE   // disable some Automaton Ardunio issues (noTone, Tone, AnalogWrite)
#define SDA         21
#define SCL         22
#define LOX_GPIO    13    // D7
#define RELAY_GPIO  12    // D6
#define RELAY_PIN    5    // sonoff
#define DHT_PIN      4    // D4  4
#define LD_IO       18    // D18        
#define LD_RX       16    // D6
#define LD_TX       17    // D7
#define DHT_TYPE DHTesp::DHT_MODEL_t::DHT22 // DHTesp::DHT22

#define SENSOR_READ_INTERVAL 300
#define LD2410_TARGET_REPORTING false   // enables status property

#define LOX_TIMING_BUDGET_US    250000        // = 250 ms
#define LOX_INTERMEASUREMENT_MS   1000        // = 1000 ms

#define VALIDATION_MAX_MS         1000
#define VALIDATION_MIN_MS          200
#define VALIDATION_DEFAULT_MS      400 

// guard-flag to prevent sending properties when offline
volatile bool gbSensorsEnabled=false;

SknLD2410    presence(LD_RX, LD_TX, LD_IO);
SknDHT    environment(DHT_PIN);
SknVL53L1x        lox(LOX_TIMING_BUDGET_US, LOX_INTERMEASUREMENT_MS);
SknRelay        relay(RELAY_PIN, VALIDATION_DEFAULT_MS, HIGH);

void setup() {
  delay(250);  
  Serial.begin(115200);
  delay(250);  
  Serial << endl << "Starting..." << endl;
  
  // enable all classes
  gbSensorsEnabled = true;
}

void loop() {
  // put your main code here, to run repeatedly:
}