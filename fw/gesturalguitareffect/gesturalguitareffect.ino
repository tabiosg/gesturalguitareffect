#include <Arduino.h>
#include <AudioStream.h>

#define SAMPLE_CYCLES 128

#include "accelerometer.h"
#include "guitareffects.h"
#include "audiogestural.h"

#include <Wire.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

// Define the audio connections
AudioInputI2S            input;        // Input from audio codec
AudioOutputI2S           output;       // Output to audio codec
AudioControlSGTL5000     codec;        // Audio codec control

// SDA is 18, SCL is 19  - https://www.pjrc.com/teensy/td_libs_Wire.html

#define NONE_PIN 29
#define TREMOLO_PIN 30
#define DELAY_PIN 31
#define FUZZ_PIN 32

AudioEffectGesture gAudioEffectGesture;
Accelerometer gAccel;

// Define audio connections using AudioConnection
AudioConnection          patchCord1(input, 0, gAudioEffectGesture, 0);
AudioConnection          patchCord2(gAudioEffectGesture, 0, output, 0);
AudioConnection          patchCord3(input, 1, gAudioEffectGesture, 0);
AudioConnection          patchCord4(gAudioEffectGesture, 0, output, 1);

void setup() {
  Wire.begin(); // Initialize I2C communication
  Serial.begin(9600); // Initialize Serial communication

   // Initialize the audio shield and codec
  AudioMemory(12);
  codec.enable();
  codec.inputSelect(AUDIO_INPUT_LINEIN);  // Set input source (e.g., LINE-IN or MIC)
  codec.volume(0.6);                      // Set volume level

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
