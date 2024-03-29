#include "audiogestural.h"

AudioEffectGesture::AudioEffectGesture()
  : AudioStream(1, inputQueueArray) {
  mCurrentEffect = GuitarEffect::None;
  memset(mDelayBuffer, 0, DELAY_LENGTH * sizeof(int16_t));
  mWriteIndex = 0;

  mDepth = 0.5;
  mRate = MAX_RATE;

  mDelayRatios[0] = 0.65f;
  for (int i = 1; i < MAX_NUMBER_DELAY_REPEATS; ++i) {
      mDelayRatios[i] = mDelayRatios[i - 1] * 0.65;
  }

  mCurrentNumberDelayRepeats = MAX_NUMBER_DELAY_REPEATS;
  mCurrentDelayStepSize = MAX_DELAY_STEP_SIZE;
}

void AudioEffectGesture::updatePotentiometer(float value) {
  // Expecting between 0.0 and 1.0
  // Tremolo effect
  mDepth = value / 2.0;  // map to 0 and 0.5

  mCurrentNumberDelayRepeats = map(value * 10, 0, 10, MIN_NUMER_DELAY_REPEATS, MAX_NUMBER_DELAY_REPEATS);

  #ifdef DEBUG
  String output = "$POTENTSCALEDTODEPTH," + String(value) + "," + String(mDepth) + ",";
  Serial.println(output);
  output = "$POTENTSCALEDTOREPEATS," + String(value) + "," + String(mCurrentNumberDelayRepeats) + ",";
  Serial.println(output);
  // Delay effect
  #endif
}

void AudioEffectGesture::updateAccelerometer(float value) {
  static float prev_value_used_for_step_size = 420; 

  // Expecting between -90 and 90. But it's more like -50 and 50
  float accel_min = -50;
  float accel_max = 50;
  float orig_value = value;
  value = value < accel_min ? accel_min : value;
  value = value > accel_max ? accel_max : value;
  
  int min = MIN_RATE;
  int max = MAX_RATE;

  mRate = min + map(value, accel_min, accel_max, 0, max - min);

  int possibleCurrentDelayStepSize = map(value, accel_min, accel_max, 100, DELAY_LENGTH / (mCurrentNumberDelayRepeats - 1)); // MAX_DELAY_STEP_SIZE);
  if (abs(prev_value_used_for_step_size - value) > 10) {
    mCurrentDelayStepSize = possibleCurrentDelayStepSize;
    prev_value_used_for_step_size = value;
  }

  #ifdef DEBUG
  String output = "$ACCELSCALEDTORATE," + String(orig_value) + "," + String(mRate) + ",";
  Serial.println(output);
  output = "$ACCELSCALEDTOPOSSIBLEDELAY," + String(orig_value) + "," + String(possibleCurrentDelayStepSize) + ",";
  Serial.println(output);
  output = "$ACCELSCALEDTODELAYLENGTH," + String(orig_value) + "," + String(mCurrentDelayStepSize) + ",";
  Serial.println(output);
  #endif
}

void AudioEffectGesture::changeEffect(GuitarEffect effect) {
  mCurrentEffect = effect;
}

void AudioEffectGesture::update(void) {

  if (mCurrentEffect == GuitarEffect::None) {
    audio_block_t *block;
    // Check if input data is available
    block = receiveReadOnly(0);
    if (block != NULL) {
      updateDelayBuffer(block);  // used for Delay effect
      // Pass the input data to the output
      transmit(block);
      // Release the input data block
      release(block);
    }
  } else if (mCurrentEffect == GuitarEffect::Tremolo) {
    audio_block_t *block;
    // Check if input data is available
    block = receiveReadOnly(0);
    if (block != NULL) {

      updateDelayBuffer(block);  // used for Delay effect

      updateTremolo(block);
      // Pass the input data to the output
      transmit(block);
      // Release the input data block
      release(block);
    }
  } else if (mCurrentEffect == GuitarEffect::Delay) {
    audio_block_t *block;
    // Check if input data is available
    block = receiveReadOnly(0);
    if (block != NULL) {
      // Pass the input data to the output with delay effect
      applyDelay(block);
      // Release the input data block
      release(block);
    }
  }
}

void AudioEffectGesture::applyDelay(audio_block_t *block) {
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    int16_t input = block->data[i];

    block->data[i] = input * mDelayRatios[0];
    int currentIndex = mWriteIndex - mCurrentDelayStepSize;
    for (int j = 1; j < mCurrentNumberDelayRepeats; ++j) {
      while (currentIndex < 0) {
        currentIndex += DELAY_LENGTH;
      }
      block->data[i] += mDelayBuffer[currentIndex] * mDelayRatios[j];
      currentIndex -= mCurrentDelayStepSize;
    }
    mDelayBuffer[mWriteIndex] = input;
    if (mWriteIndex == (DELAY_LENGTH - 1)) {
      mWriteIndex = 0;
    }
    else {
      mWriteIndex += 1;
    }
  }
  // Output the delayed audio
  transmit(block);
}

void AudioEffectGesture::updateTremolo(audio_block_t *block) {
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float sincalc = sin(0.00014240362 * mWriteIndex * mRate); // sin(2 * M_PI * static_cast<float>(mWriteIndex) / DELAY_LENGTH * mRate);
    float factor = (1.0 - mDepth) + mDepth * sincalc;
    block->data[i] *= factor;
  }
}

void AudioEffectGesture::updateDelayBuffer(audio_block_t *block) {
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    int16_t input = block->data[i];
    mDelayBuffer[mWriteIndex] = input;
    if (mWriteIndex == (DELAY_LENGTH - 1)) {
      mWriteIndex = 0;
    }
    else {
      mWriteIndex += 1;
    }
  }
}
