#include "audiogestural.h"

AudioEffectGesture::AudioEffectGesture()
  : AudioStream(1, inputQueueArray) {
  mCurrentEffect = GuitarEffect::None;
  memset(mDelayBuffer, 0, DELAY_LENGTH * sizeof(int16_t));
  mWriteIndex = 0;

  mDepth = 0.5;
  mRate = MAX_RATE;

  mDelayRatios[0] = 0.45f;
  for (int i = 1; i < MAX_NUMBER_DELAY_REPEATS; ++i) {
      mDelayRatios[i] = mDelayRatios[i - 1] * 0.45;
  }

  mCurrentNumberDelayRepeats = MAX_NUMBER_DELAY_REPEATS;
  mCurrentDelayStepSize = MAX_DELAY_STEP_SIZE;

  mCurrentGain = 20.0f;
  mCurrentCenterFrequency = 1000;
  peakingCoefficients(mCurrentGain, mCurrentCenterFrequency, 1.9, 44100.0f);
}

void AudioEffectGesture::updatePotentiometer(float value) {
  // Expecting between 0.0 and 1.0

  #ifdef DEBUG
  String output = "$POTENTSCALEDTODEPTH," + String(value) + "," + String(mDepth) + ",";
  Serial.println(output);
  output = "$POTENTSCALEDTOREPEATS," + String(value) + "," + String(mCurrentNumberDelayRepeats) + ",";
  Serial.println(output);
  // Delay effect
  #endif

  float input = value * 2 - 1.0;
  
  if (mCurrentEffect == GuitarEffect::Tremolo) {
    mDepth = calculateDepth(input);  // map to 0 and 0.5
  } else if (mCurrentEffect == GuitarEffect::Delay) {
    mCurrentNumberDelayRepeats = calculateRepeats(input);
  } else if (mCurrentEffect == GuitarEffect::Wah) {
    mCurrentGain = calculateGain(input);
  }

}

float AudioEffectGesture::convertAccelValueToInput(float value) {
  static float last_angle = value;
  static int calls_to_wait = 0;

  float usable_angle = value;

  usable_angle = usable_angle < -ACCEL_MAX_DEG_FOR_INPUT ? -ACCEL_MAX_DEG_FOR_INPUT : usable_angle;
  usable_angle = usable_angle > ACCEL_MAX_DEG_FOR_INPUT ? ACCEL_MAX_DEG_FOR_INPUT : usable_angle;

  bool should_use_last_angle = false;

  if (calls_to_wait > 0) {
    --calls_to_wait;
    should_use_last_angle = true;
  }
  else if (usable_angle == -ACCEL_MAX_DEG_FOR_INPUT || usable_angle == ACCEL_MAX_DEG_FOR_INPUT) {
    should_use_last_angle = false;
  }
  else if (fabs(last_angle - usable_angle) < ACCEL_DEG_THRESHOLDING) {
    should_use_last_angle = true;
  }

  if (should_use_last_angle) {
    usable_angle = last_angle;
  }
  else {
    calls_to_wait = ACCEL_CALLS_TO_WAIT_BEFORE_UPDATING_AGAIN;
    last_angle = usable_angle;
  }

  float input = usable_angle / ACCEL_MAX_DEG_FOR_INPUT;
  
  return input;
}

void AudioEffectGesture::updateAccelerometer(float value) {

  float input = convertAccelValueToInput(value);

  if (mCurrentEffect == GuitarEffect::Tremolo) {
    mRate = calculateRate(input);
  } else if (mCurrentEffect == GuitarEffect::Delay) {
    mCurrentDelayStepSize = calculateStepSize(input);
  } else if (mCurrentEffect == GuitarEffect::Wah) {
    mCurrentCenterFrequency = calculateCenterFrequency(input);
  }

  #ifdef DEBUG
  String output = "$ACCELSCALEDTORATE," + String(value) + "," + String(mRate) + ",";
  Serial.println(output);
  output = "$ACCELSCALEDTOPOSSIBLEDELAY," + String(value) + "," + String(possibleCurrentDelayStepSize) + ",";
  Serial.println(output);
  output = "$ACCELSCALEDTODELAYLENGTH," + String(value) + "," + String(mCurrentDelayStepSize) + ",";
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

      int startingIndex = mWriteIndex;

      updateDelayBuffer(block);  // used for Delay effect

      updateTremolo(block, startingIndex);
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
            b_new->data[i] = (int16_t)(output[i] * 32768.0f * 0.25); // Convert back to 16-bit PCM
        }

        transmit(b_new);
        release(b_new);
      }
      // Release the input data block
      release(block);
    }
  }
}
