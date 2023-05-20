#ifndef LILYGOA7608_H_
#define LILYGOA7608_H_

#include "esp_sleep.h"

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
// MODEM POWER FUNCTIONS                          //
////////////////////////////////////////////////////

// Sorts data in ascending order
template <typename T>
void bubbleSort(T data[], int8_t N) {
    T temp;
    for (int8_t i = 0; i < (N - 1); i++) {
        for (int8_t o = 0; o < (N - (i + 1)); o++) {
            if (data[o] > data[o + 1]) {
                temp = data[o];
                data[o] = data[o + 1];
                data[o + 1] = temp;
            }
        }
    }
}

// get battery voltage
float batteryVoltage(int N = 5) {
    // read battery voltage
    pinMode(BAT_ADC, INPUT);

    float samples[N];
    float voltage = 0.0;

    // get samples
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            samples[j] = (float)analogReadMilliVolts(BAT_ADC) * 2 / 1000;
            delay(2);
        }

        // sort data
        bubbleSort(samples, N);

        // average N times the median values of N samples
        voltage += samples[(N-1) / 2] / N;
    }

    // return median  
    return voltage;
}

// enable battery power
void enable_battery_functions() {
    pinMode(BAT_EN, OUTPUT);
    digitalWrite(BAT_EN, HIGH);
}

// disable battery
void disable_battery_functions() {
    pinMode(BAT_EN, OUTPUT);
    digitalWrite(BAT_EN, LOW);
}

// A7608 Reset
// see simcom-A7608 hardware design document
void modem_hw_reset() {
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, LOW);  delay(100);
    digitalWrite(RESET, HIGH); delay(3000);
    digitalWrite(RESET, LOW);  delay(100);
}

// A7608 Power on
// see simcom-A7608 hardware design document
void modem_hw_poweron() {
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, LOW);  delay(100);
    digitalWrite(PWR_PIN, HIGH); delay(1000); // this is Ton
    digitalWrite(PWR_PIN, LOW);  delay(10000);
}

// A7608 Power off
// see simcom-A7608 hardware design document
void modem_hw_poweroff() {
    pinMode(PWR_PIN, OUTPUT);
    digitalWrite(PWR_PIN, HIGH);  delay(3000); // this is Toff
    digitalWrite(PWR_PIN, LOW);   delay(2000);
}

// avoid modem starting when waking up from sleep
void keepRESETPINLOW() {
    pinMode(RESET, OUTPUT);
    digitalWrite(RESET, LOW);
    gpio_hold_en(GPIO_NUM_5);
    gpio_deep_sleep_hold_en();
}


#endif LILYGOA7608_H_