#include <PWM.h>

const int signal_pin = 9;
int32_t frequency = 201;

void setup() {
  InitTimersSafe();
  pinMode(signal_pin, OUTPUT);
}

void loop() {
  pwmWriteHR(signal_pin, 32768);  // Set duty cycle to 50% by default -> for 16-bit 65536/2 = 32768
  SetPinFrequencySafe(signal_pin, frequency);
}
