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
#include "DHTNode.hpp"
#include "LD2410Client.hpp"
#include "SknLoxRanger.hpp"


#ifndef ARDUINO_ESP32_DEV
extern "C"
{
#include "user_interface.h" // to set CPU Freq for Non-Huzzah's
}
#endif

// Select pins for Temperature and Motion
#define PIN_DHT  4         // D4  4
#define LD_IO   18         // D18        
#define LD_RX   16         // D6
#define LD_TX   17         // D7
#define DHT_TYPE DHTesp::DHT_MODEL_t::DHT22 // DHTesp::DHT22

#define SENSOR_READ_INTERVAL 300
#define LD2410_TARGET_REPORTING true   // enables status property

#define LOX_TIMING_BUDGET_US    250000        // = 250 ms
#define LOX_INTERMEASUREMENT_MS    500        // = 500 ms

// esp32doit-devkit-v1  pins
  #define SDA 21
  #define SCL 22
  #define ATM_PLAYER_DISABLE_TONE   // disable some Automaton Ardunio issues (noTone, Tone, AnalogWrite)
  #define LOX_GPIO   13    // D7
  #define RELAY_GPIO 12    // D6
  #undef LED_BUILTIN
  #define LED_BUILTIN 4

// guard-flag to prevent sending properties when offline
volatile bool gbEnableDoorOperations=false;


void setup() {
  delay(250);  
  Serial.begin(115200);
  delay(250);  
  Serial << endl << "Starting..." << endl;

}

void loop() {
  // put your main code here, to run repeatedly:
}