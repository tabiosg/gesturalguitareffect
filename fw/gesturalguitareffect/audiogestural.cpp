#include "audiogestural.h"

AudioEffectGesture::AudioEffectGesture()
  : AudioStream(1, inputQueueArray) {
  mCurrentEffect = GuitarEffect::None;
  // mCurrentEffect = GuitarEffect::Delay;
  memset(mDelayBuffer, 0, DELAY_LENGTH * sizeof(int16_t));
  mWriteIndex = 0;

  float sum = 0;
  mDelayRatios[0] = 1.0f;
    for (int i = 1; i < NUMBER_DELAY_REPEATS; ++i) {
        mDelayRatios[i] = mDelayRatios[i - 1] * 0.8;
        sum += mDelayRatios[i];  // Values: 1, 2, 4, 8, 16, ...
    }

    for (int i = 0; i < NUMBER_DELAY_REPEATS; ++i) {
        mDelayRatios[i] /= sum;
    }
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
      updateDelayBuffer(block);
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
    for (int j = 0; j < NUMBER_DELAY_REPEATS - 1; ++j) {
      block->data[i] += mDelayBuffer[(mWriteIndex + (DELAY_LENGTH * j / NUMBER_DELAY_REPEATS)) % DELAY_LENGTH] * mDelayRatios[j];
    }
    mDelayBuffer[mWriteIndex] = input;
    mWriteIndex = (mWriteIndex + 1) % DELAY_LENGTH;
    block->data[i] += input; // * mDelayRatios[NUMBER_DELAY_REPEATS - 1];
  }
  // Output the delayed audio
  transmit(block);
}

void AudioEffectGesture::updateDelayBuffer(audio_block_t *block) {
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    int16_t input = block->data[i];
    mDelayBuffer[mWriteIndex] = input;
    mWriteIndex = (mWriteIndex + 1) % DELAY_LENGTH;
  }
}