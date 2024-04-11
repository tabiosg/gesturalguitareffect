#include "audiogestural.h"

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