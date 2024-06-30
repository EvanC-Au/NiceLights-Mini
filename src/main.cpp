/*  NiceLights Mini 
    ----------------
    Evan Cottle
    evan@cottle.au
    ----------------
    H-Bridge direct driver for mini solar lights for ATTiny85 
    Drives LEDs at higher than standard frequency, with a subtle "breathing" effect
    Output via PWM timer 1 A/B - connect LED string directly across these pins
    Alternatively, use this to drive a MOSFET-based circuit for additional current or voltage

               -------------
              |      U      |
       NC ----| 1         5 |---- VCC (5V)
       NC ----| 2         6 |---- NC
    OUT B ----| 3         7 |---- OUT A
      GND ----| 4         8 |---- NC
               -------------
    */


#include <Arduino.h>

// Clear and set bit macros from wiring_analog.c
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Lookup table for wave function
// Spreadsheet function: =round(60*sin(pi()*A1/127.5)+60)
const uint8_t lut[256] = {
    60,61,63,64,66,67,69,70,72,73,75,76,77,79,80,82,83,84,86,87,88,90,91,92,93,95,96,97,98,99,100,
    102,103,104,105,106,107,107,108,109,110,111,112,112,113,114,114,115,116,116,117,117,118,118,118,
    119,119,119,119,120,120,120,120,120,120,120,120,120,120,119,119,119,119,118,118,118,117,117,116,
    116,115,115,114,113,113,112,111,110,110,109,108,107,106,105,104,103,102,101,100,99,98,96,95,94,
    93,92,90,89,88,86,85,84,82,81,80,78,77,75,74,72,71,70,68,67,65,64,62,61,59,58,56,55,53,52,50,49,
    48,46,45,43,42,40,39,38,36,35,34,32,31,30,28,27,26,25,24,22,21,20,19,18,17,16,15,14,13,12,11,10,
    10,9,8,7,7,6,5,5,4,4,3,3,2,2,2,1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,3,3,4,4,5,6,6,7,8,8,9,10,
    11,12,13,13,14,15,16,17,18,20,21,22,23,24,25,27,28,29,30,32,33,34,36,37,38,40,41,43,44,45,47,48,50,51,53,54,56,57,59,60};

void setup()
{
  // Set pin modes to output
  pinMode(PB4, OUTPUT);
  pinMode(PB1, OUTPUT);

  // Set output A to PB1, inverted
  sbi(TCCR1, COM1A1);
  sbi(TCCR1, COM1A0);

  // Set output B to PB4
  sbi(GTCCR, COM1B1);

  // Disable prescaler 
  cbi(TCCR1, CS13);
  cbi(TCCR1, CS12);
  cbi(TCCR1, CS11);
  sbi(TCCR1, CS10);

  OCR1C = 127;    // Set PWM frequency - works out to about 8kHz on default hardware

  OCR1A = 127;    // Initial 50% duty cycle
  OCR1B = 127; 
}

void loop()
{
  // Select entry from wave LUT using bits of millis()
  uint8_t x = lut[(millis() >> 1) & 0xFF];

  // Use LUT value to offset output value
  uint8_t i = 100 + x;
  uint8_t j = 255 - (100 + (100 - x));

  // Write out duty cycle to PWM
  OCR1A = i >> 1;
  OCR1B = j >> 1;

  // No point hammering hard, a small delay should hopefully improve battery life
  // Still going to get > 100FPS
  delay(1);
}
