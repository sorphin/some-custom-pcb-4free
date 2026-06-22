/** 
Wiring the ADS1115 Module to an ESP32 C3 Super MINi
  VDD        5V
  GND        GND
  SCL        6 (or SCL)
  SDA        5 (or SDA)
  ADDR       TO GND 
*/

#include <Arduino.h>
#include "ADS1115.h"

/** Set moduke i2c address */
 ADS1115 adc0 (ADS1115_ADDRESS_ADDR_GND);
 /** Init */
void initADS1115(){
    adc0.initialize();
    adc0.setMode(ADS1115_MODE_SINGLESHOT);
    adc0.setRate(ADS1115_RATE_32);
    adc0.setGain(ADS1115_PGA_4P096);
    adc0.setConversionReadyPinMode();
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    initADS1115();

}
    

void loop (){
    // A0
    adc0.setMultiplexer(ADS1115_MUX_P0_NG);
    adc0.triggerConversion(); // arlet PIN
    int16_t raw0 = adc0.getConversion();
    //A1
    adc0.setMultiplexer(ADS1115_MUX_P1_NG);
    adc0.triggerConversion();
    int16_t raw1 = adc0.getConversion();
    //A2
    adc0.setMultiplexer(ADS1115_MUX_P2_NG);
    adc0.triggerConversion(); // arlet PIN
    int16_t raw2 = adc0.getConversion();
    //A3
    adc0.setMultiplexer(ADS1115_MUX_P3_NG);
    adc0.triggerConversion(); // arlet PIN
    int16_t raw3 = adc0.getConversion();

    Serial.print("ADC A0: "); Serial.print (raw0);
    Serial.print("\tADC A1: "); Serial.println (raw1);

    Serial.print("ADC A2: "); Serial.print (raw2);
    Serial.print("\tADC A3: "); Serial.println (raw3);

    delay(1000);


}



