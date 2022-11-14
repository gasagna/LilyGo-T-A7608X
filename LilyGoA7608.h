#ifndef LILYGOA7608_H_
#define LILYGOA7608_H_

////////////////////////////////////////////////////
// PINS                                           //
////////////////////////////////////////////////////

// configuration for serial port to simcom module
#define PIN_TX   26
#define PIN_RX   27

// for the battery
#define BAT_EN   12
#define BAT_ADC  35 // read battery voltage, Disabled when using USB - ONLY INPUT PIN

// modem pins
#define PWR_PIN  4
#define RESET    5

// sd card pins
#define SD_MISO  2
#define SD_MOSI  15
#define SD_SCLK  14
#define SD_CS    13

////////////////////////////////////////////////////
// OTHER FUNCTIONALITY                            //
////////////////////////////////////////////////////

#include "LilyGoA7608_POWER.h"
#include "LilyGoA7608_MQTT.h"

#endif LILYGOA7608_H_