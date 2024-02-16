#include <PWM.h>

const int signal_pin = 9;
int32_t frequency = 2500;

void setup() {
  Serial.begin(9600);
  InitTimersSafe();
  pinMode(signal_pin, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    int32_t newFrequency = Serial.parseInt();

    // Input Validation (Optional but recommended)
    if (newFrequency > 0) {  // Adjust validation as needed
       frequency = newFrequency; 
    }
  }
  pwmWriteHR(signal_pin, 32768);  // Set duty cycle to 50% by default -> for 16-bit 65536/2 = 32768
  SetPinFrequencySafe(signal_pin, frequency);
}

