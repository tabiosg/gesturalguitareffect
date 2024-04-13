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
  int numSamplesToEnd = DELAY_LENGTH - mWriteIndex;
  int numSamplesToCopy = AUDIO_BLOCK_SAMPLES;
  if (numSamplesToCopy > numSamplesToEnd)
      numSamplesToCopy = numSamplesToEnd;

  memcpy(&mDelayBuffer[mWriteIndex], block->data, numSamplesToCopy * sizeof(int16_t));
  if (numSamplesToCopy < AUDIO_BLOCK_SAMPLES) {
      memcpy(mDelayBuffer, &block->data[numSamplesToCopy], (AUDIO_BLOCK_SAMPLES - numSamplesToCopy) * sizeof(int16_t));
  }

  mWriteIndex += AUDIO_BLOCK_SAMPLES;
  if (mWriteIndex >= DELAY_LENGTH) {
      mWriteIndex -= DELAY_LENGTH;
  }
}

int AudioEffectGesture::calculateRepeats(float input) {
  return map(input * REPEATS_RESOLUTION, -REPEATS_RESOLUTION, REPEATS_RESOLUTION, MIN_NUMER_DELAY_REPEATS, MAX_NUMBER_DELAY_REPEATS);
}

int AudioEffectGesture::calculateStepSize(float input) {
  return map(input * STEP_SIZE_RESOLUTION, -STEP_SIZE_RESOLUTION, STEP_SIZE_RESOLUTION, 100, DELAY_LENGTH / (mCurrentNumberDelayRepeats - 1));
}