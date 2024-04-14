#include <Arduino.h>
#include <AudioStream.h>

#define SAMPLE_CYCLES 128

#include "accelerometer.h"
#include "guitareffects.h"
#include "audiogestural.h"
#include "potentiometer.h"

#include <Wire.h>

#include <Audio.h>
#include <SPI.h>

// Define the audio connections
AudioInputI2S            input;        // Input from audio codec
AudioOutputI2S           output;       // Output to audio codec
AudioControlSGTL5000     codec;        // Audio codec control

// Wire: SDA is 18, SCL is 19  - https://www.pjrc.com/teensy/td_libs_Wire.html
// Wire2: SDA is 25, SCL is 24  - https://www.pjrc.com/teensy/td_libs_Wire.html

#define NONE_PIN 34 
#define TREMOLO_PIN 33
#define DELAY_PIN 35
#define WAH_PIN 31

AudioEffectGesture gAudioEffectGesture;
Accelerometer gAccel;
Potentiometer gPotentiometer;

// Define audio connections using AudioConnection
AudioConnection          patchCord1(input, 0, gAudioEffectGesture, 0);

AudioConnection          patchCord3(gAudioEffectGesture, 0, output, 0);
AudioConnection          patchCord4(gAudioEffectGesture, 0, output, 1);

void setup() {
  Serial.begin(9600); // Initialize Serial communication

   // Initialize the audio shield and codec
  AudioMemory(12);
  codec.enable();
  codec.inputSelect(AUDIO_INPUT_LINEIN);  // Set input source (e.g., LINE-IN or MIC)
  codec.volume(0.6);                      // Set volume level

  delay(1000); // Delay for stability

  pinMode(NONE_PIN, INPUT_PULLUP);
  pinMode(TREMOLO_PIN, INPUT_PULLUP);
  pinMode(DELAY_PIN, INPUT_PULLUP);
  pinMode(WAH_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(NONE_PIN), doNoneEffect, RISING);
  attachInterrupt(digitalPinToInterrupt(TREMOLO_PIN), doTremoloEffect, RISING);
  attachInterrupt(digitalPinToInterrupt(DELAY_PIN), doDelayEffect, RISING);
  attachInterrupt(digitalPinToInterrupt(WAH_PIN), doWahEffect, RISING);

  gAccel.setup();
}

void loop() {
  #ifdef DEBUG
  gAudioEffectGesture.printEffect();
  #endif
  __disable_irq();
  float potentData = gPotentiometer.getConvertedData();
  gAudioEffectGesture.updatePotentiometer(potentData); 
  float accelData = gAccel.getZDegrees();
  gAudioEffectGesture.updateAccelerometer(accelData);
  gAudioEffectGesture.applyPeakingCoefficients();
  __enable_irq();
  delay(100); // Delay before next iteration
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

void doWahEffect() {
  gAudioEffectGesture.changeEffect(GuitarEffect::Wah);
}
