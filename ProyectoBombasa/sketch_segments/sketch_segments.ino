
#include <Arduino.h>
#include <TM1637Display.h>

#define CLK 11
#define DIO 10
TM1637Display display(CLK, DIO);
int i;

const uint8_t SEG_MAN[] = {
  SEG_E | SEG_F | SEG_A | SEG_B | SEG_C,                    // M
  SEG_F | SEG_A | SEG_B | SEG_C | SEG_E,                    // 
  SEG_C | SEG_E | SEG_G | SEG_F | SEG_A | SEG_B,            // A
  SEG_F | SEG_A | SEG_E | SEG_F | SEG_B | SEG_C,            // N
};

const uint8_t SEG_AUTO[] = {
  SEG_C | SEG_E | SEG_G | SEG_F | SEG_A | SEG_B,            // A
  SEG_E | SEG_F | SEG_D | SEG_B | SEG_C,                    // U
  SEG_A | SEG_B | SEG_C,                                    // T
  SEG_A | SEG_E | SEG_F | SEG_B | SEG_C | SEG_D,            // O
};

const uint8_t SEG_OCHO[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,    // 8
};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  display.setBrightness(0x0f);

}

void loop() {
  // put your main code here, to run repeatedly:
 display.setSegments(SEG_MAN);

 delay(3000);

 display.setSegments(SEG_AUTO);

 delay(5000);

 display.setSegments(SEG_OCHO);

 delay(5000);
 
  }
