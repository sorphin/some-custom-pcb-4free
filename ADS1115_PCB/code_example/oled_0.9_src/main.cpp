
/** 
Wiring the ADS1115 Module to an ESP32 C3 Super MINi
  VDD        5V
  GND        GND
  SCL        6 (or SCL)
  SDA        5 (or SDA)
  ADDR       TO SDA
*/
/** 
 * TẤT CẢ NHŨNG PHẦN MÔ TẢ BÊN DƯỚI ĐỀU CÓ TRONG DATASHEET <page 25-26> -> https://www.ti.com/lit/ds/symlink/ads1115.pdf?ts=1756774427639
 * ADS1115 có độ phân giải 16-bit signed → giá trị dao động -32768 … +32767
 * Nếu ae đang đọc single-ended (A0,A1... vs GND) thì giá trị nằm từ 0 … 32767.
 * Mình đang set ADS1115_PGA_4P096 (ESP32 GPIO 3.3V) → nó đo được tới ±4.1V
 * Code hiện tại đang set PGA = ±4.096V. → tức là full-scale (FSR) = 4.096V
 * Cách tính điện áp từ ADC -> Vin ​= (ADC/32767)​×FSR
 * 
 */

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "ADS1115.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
ADS1115 adc0(ADS1115_ADDRESS_ADDR_GND);

// Layout 4 kênh: mỗi hàng cao 15px, cách nhau 1px
// Frame: x=28, w=96, h=13
// Baseline text label: y = frameY + 11
struct ChannelLayout {
    uint8_t frameY;
    uint8_t labelY;
};


const ChannelLayout CH[4] = {
    {1,  12},
    {16, 27},
    {31, 42},
    {46, 57},
};

const char* chLabel[4] = {"CH1","CH2","CH3","CH4"};

// FSR theo PGA_4P096
const float FSR = 4.096f;

float rawToVolt(int16_t raw) {
    // Single-ended: 0..32767 → 0..FSR
    if (raw < 0) raw = 0;
    return (raw / 32767.0f) * FSR; // Vin = (ADC/32767)×FSR
}

void drawStaticLayout() {
    u8g2.setFont(u8g2_font_t0_11_tr); 
    for (int i = 0; i < 4; i++) {
        u8g2.drawFrame(28, CH[i].frameY, 96, 13);
        u8g2.drawStr(2, CH[i].labelY, chLabel[i]);
    }
}

void updateChannelValue(uint8_t ch, float volt) {
    char buf[12];
    dtostrf(volt, 5, 3, buf); // "x.xxx"
    strcat(buf, "V");         // "x.xxxV"
    Serial.printf("CH%d: %s\n", ch+1, buf);

  
    u8g2.setDrawColor(0); 
    u8g2.drawBox(29, CH[ch].frameY + 1, 94, 11);
    u8g2.setDrawColor(1);

    u8g2.setFont(u8g2_font_t0_11_tr);
    u8g2.drawStr(50, CH[ch].labelY, buf);
}

void InitADS() {
    adc0.initialize();
    adc0.setMode(ADS1115_MODE_SINGLESHOT);
    adc0.setRate(ADS1115_RATE_32);
    adc0.setGain(ADS1115_PGA_4P096);
    adc0.setConversionReadyPinMode();
}

int16_t readChannel(uint8_t mux) {
    const uint8_t muxList[4] = {
        ADS1115_MUX_P0_NG,
        ADS1115_MUX_P1_NG,
        ADS1115_MUX_P2_NG,
        ADS1115_MUX_P3_NG,
    };
    adc0.setMultiplexer(muxList[mux]);
    adc0.triggerConversion();
    return adc0.getConversion();
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    u8g2.begin();

    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    drawStaticLayout();
    u8g2.sendBuffer();

    InitADS();
}

void loop() {
    for (uint8_t i = 0; i < 4; i++) {
        int16_t raw = readChannel(i);
        float volt  = rawToVolt(raw);

        updateChannelValue(i, volt);

        Serial.printf("CH%d: raw=%d  %.3fV\n", i+1, raw, volt);
    }

    u8g2.sendBuffer();

    delay(500);
}