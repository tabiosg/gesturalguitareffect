#include "audiogestural.h"

AudioEffectGesture::AudioEffectGesture()
  : AudioStream(1, inputQueueArray) {
  mCurrentEffect = GuitarEffect::None;
  // mCurrentEffect = GuitarEffect::Delay;
  memset(mDelayBuffer, 0, DELAY_LENGTH * sizeof(int16_t));
  mWriteIndex = 0;

  mDepth = 0.5;
  mRate = 10;

  updateNumberDelayRepeats(3);
  mCurrentDelayLength = DELAY_LENGTH;
}

void AudioEffectGesture::updatePotentiometer(float value) {
  // Expecting between 0.0 and 1.0
  // Tremolo effect
  mDepth = value;
  mCurrentNumberDelayRepeats = value * 10;
  String output = "$POTENTRAWTODEPTH," + String(value) + "," + String(mDepth) + ",";
  Serial.println(output);
  output = "$POTENTRAWTOREPEATS," + String(value) + "," + String(mCurrentNumberDelayRepeats) + ",";
  Serial.println(output);
  // Delay effect
}

void AudioEffectGesture::updateAccelerometer(float value) {
  // Expecting between -90 and 90. But it's more like -50 and 50
  float accel_min = -50;
  float accel_max = 50;
  float orig_value = value;
  value = value < accel_min ? accel_min : value;
  value = value > accel_max ? accel_max : value;
  
  int min = 1;
  int max = 20;

  mRate = min + map(value, accel_min, accel_max, 0, max - min);

  int possibleDelay = map(value / 20, accel_min / 20, accel_max / 20, 1000, DELAY_LENGTH);
  if (abs(possibleDelay - mCurrentDelayLength) > 4000) {
    mCurrentDelayLength = possibleDelay;
  }

  String output = "$ACCELRAWTORATE," + String(orig_value) + "," + String(mRate) + ",";
  Serial.println(output);
  output = "$ACCELRAWTOPOSSIBLEDELAY," + String(orig_value) + "," + String(possibleDelay) + ",";
  Serial.println(output);
  output = "$ACCELRAWTODELAYLENGTH," + String(orig_value) + "," + String(mCurrentDelayLength) + ",";
  Serial.println(output);
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
    // int16_t output = mDelayBuffer[mWriteIndex];

    // block->data[i] = (1 - mDelayMixRatio) * input + mDelayMixRatio * output;
    block->data[i] = 0;
    for (int j = 0; j < mCurrentNumberDelayRepeats - 1; ++j) {
      block->data[i] += mDelayBuffer[(mWriteIndex + (DELAY_LENGTH - mCurrentDelayLength) + (mCurrentDelayLength * j / mCurrentNumberDelayRepeats)) % DELAY_LENGTH] * mDelayRatios[j];
    }
    mDelayBuffer[mWriteIndex] = input;
    mWriteIndex = (mWriteIndex + 1) % DELAY_LENGTH;
    block->data[i] += input;
  }
  // Output the delayed audio
  transmit(block);
}

void AudioEffectGesture::updateTremolo(audio_block_t *block) {
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    Serial.println(mRate);
    float sincalc =  sin(2 * M_PI * static_cast<float>(mWriteIndex) / DELAY_LENGTH * mRate);
    float factor = 1.0 + mDepth * sincalc; // TODO
    block->data[i] *= factor;
  }
}

void AudioEffectGesture::updateDelayBuffer(audio_block_t *block) {
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    int16_t input = block->data[i];
    mDelayBuffer[mWriteIndex] = input;
    mWriteIndex = (mWriteIndex + 1) % DELAY_LENGTH;
  }
}

void AudioEffectGesture::updateNumberDelayRepeats(int new_num) {
  if (new_num == mCurrentNumberDelayRepeats) {
    return;
  }
  if (new_num < 0 || new_num >= MAX_NUMBER_DELAY_REPEATS) {
    return;
  }

  mCurrentNumberDelayRepeats = new_num;

  float sum = 0;
  mDelayRatios[0] = 1.0f;
  for (int i = 1; i < mCurrentNumberDelayRepeats - 1; ++i) {
      mDelayRatios[i] = mDelayRatios[i - 1] * 1.25;
      sum += mDelayRatios[i];
  }

  for (int i = 0; i < mCurrentNumberDelayRepeats - 1; ++i) {
      mDelayRatios[i] /= sum;
  }

}