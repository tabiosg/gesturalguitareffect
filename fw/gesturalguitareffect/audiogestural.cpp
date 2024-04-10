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

  mCurrentGain = 20.0f;
  mCurrentCenterFrequency = 1000;
  peakingCoefficients(mCurrentGain, mCurrentCenterFrequency, 1.9, 44100.0f);
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

  mCurrentGain = map(value * 10, 0, 10, -20, 20);
}

void AudioEffectGesture::updateAccelerometer(float value) {
  #ifdef DEBUG
  float orig_value = value;
  #endif

  static float prev_value_used_for_step_size = 420; 

  // Expecting between -90 and 90. But it's more like -50 and 50
  float accel_min = -50;
  float accel_max = 50;
  value = value < accel_min ? accel_min : value;
  value = value > accel_max ? accel_max : value;
  
  int min = MIN_RATE;
  int max = MAX_RATE;

  mRate = map(value, accel_min, accel_max, min, max);

  int possibleCurrentDelayStepSize = map(value, accel_min, accel_max, 100, DELAY_LENGTH / (mCurrentNumberDelayRepeats - 1)); // MAX_DELAY_STEP_SIZE);
  if (abs(prev_value_used_for_step_size - value) > 10) {
    mCurrentDelayStepSize = possibleCurrentDelayStepSize;
    prev_value_used_for_step_size = value;
  }

  int minCenterFrequency = 500;
  int maxCenterFrequency = 5000;
  mCurrentCenterFrequency = map(value, accel_min, accel_max, minCenterFrequency, maxCenterFrequency);

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
  } else if (mCurrentEffect == GuitarEffect::Wah) {
    audio_block_t *block;
    // Check if input data is available
    block = receiveReadOnly(0);
    if (block != NULL) {

      updateDelayBuffer(block);  // used for Delay effect

      audio_block_t *b_new;

      b_new = allocate();
      if (b_new) {
        float32_t input[AUDIO_BLOCK_SAMPLES];
        float32_t output[AUDIO_BLOCK_SAMPLES];
        
        // Copy audio block data to input buffer
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            input[i] = (float32_t)block->data[i] / 32768.0f; // Assuming 16-bit signed PCM audio
        }
        
        // Apply biquad filter
        applyBiquad(input, output, AUDIO_BLOCK_SAMPLES);
        
        // Copy filtered data back to audio block
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            b_new->data[i] = (int16_t)(output[i] * 32768.0f); // Convert back to 16-bit PCM
        }

        transmit(b_new);
        release(b_new);
      }
      // Release the input data block
      release(block);
    }
  }
}
