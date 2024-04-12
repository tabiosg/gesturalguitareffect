#include "audiogestural.h"

void AudioEffectGesture::updateTremolo(audio_block_t *block, int startingIndex) {
  int runningIndex = startingIndex;
  float mRateMultiplier = 0.00014240362 * mRate;
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float sincalc = sin(runningIndex * mRateMultiplier); // sin(2 * M_PI * static_cast<float>(mWriteIndex) / DELAY_LENGTH * mRate);
    float factor = (1.0 - mDepth) + mDepth * sincalc;
    block->data[i] *= factor;

    if (runningIndex == (DELAY_LENGTH - 1)) {
      runningIndex = 0;
    }
    else {
      runningIndex += 1;
    }
  }
}