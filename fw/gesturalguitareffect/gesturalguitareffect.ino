#include <Arduino.h>
#include <AudioStream.h>

#define SAMPLE_CYCLES 128

#include "accelerometer.h"
#include "guitareffects.h"
#include "audiogestural.h"

#include <Wire.h>

// SDA is 18, SCL is 19  - https://www.pjrc.com/teensy/td_libs_Wire.html

#define NONE_PIN 29
#define TREMOLO_PIN 30
#define DELAY_PIN 31
#define FUZZ_PIN 32

AudioEffectGesture gAudioEffectGesture;
Accelerometer gAccel;

void setup() {
  Wire.begin(); // Initialize I2C communication
  Serial.begin(9600); // Initialize Serial communication
  delay(1000); // Delay for stability

  attachInterrupt(digitalPinToInterrupt(NONE_PIN), doNoneEffect, RISING);
  attachInterrupt(digitalPinToInterrupt(TREMOLO_PIN), doTremoloEffect, RISING);
  attachInterrupt(digitalPinToInterrupt(DELAY_PIN), doDelayEffect, RISING);
  attachInterrupt(digitalPinToInterrupt(FUZZ_PIN), doFuzzEffect, RISING);

  gAccel.setup();
}

void loop() {
  Serial.println(gAccel.getZDegrees());
  // gAccel.update();

  // delay(1000); // Delay before next iteration
}

void doNoneEffect() {
  gAudioEffectGesture.changeEffect(GuitarEffect::None);
}

void doTremoloEffect() {
  gAudioEffectGesture.changeEffect(GuitarEffect::Tremolo);
}

void doDelayEffect() {
  gAudioEffectGesture.changeEffect(GuitarEffect::Delay);
}

void doFuzzEffect() {
  gAudioEffectGesture.changeEffect(GuitarEffect::Fuzz);
}
