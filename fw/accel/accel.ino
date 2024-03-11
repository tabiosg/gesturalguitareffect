#include <Arduino.h>
#include <AudioStream.h>

#define SAMPLE_CYCLES 128

#include "accelerometer.h"

#include <Wire.h>

// SDA is 18, SCL is 19  - https://www.pjrc.com/teensy/td_libs_Wire.html

Accelerometer accel;

void setup() {
  Wire.begin(); // Initialize I2C communication
  Serial.begin(9600); // Initialize Serial communication
  delay(1000); // Delay for stability

  accel.setup();
}

void loop() {
  Serial.println(accel.getZDegrees());
  // accel.update();

  // delay(1000); // Delay before next iteration
}
