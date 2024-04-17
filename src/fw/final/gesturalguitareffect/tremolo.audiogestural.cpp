#include "audiogestural.h"


void AudioEffectGesture::updateTremolo(audio_block_t *block, int startingIndex) {
  float mRateMultiplier = 0.00014240362 * mRate;
  float currentSinArgument = startingIndex * mRateMultiplier;
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float sincalc = sin(currentSinArgument); // sin(2 * M_PI * static_cast<float>(mWriteIndex) / DELAY_LENGTH * mRate);
    float factor = (1.0 - mDepth) + mDepth * sincalc;
    block->data[i] *= factor;

    currentSinArgument += mRateMultiplier;
  }
}

float AudioEffectGesture::calculateDepth(float input) {
  return map(input * RATE_RESOLUTION, -RATE_RESOLUTION, RATE_RESOLUTION, 0, 10) / 10 / 2.0;  // Needs to be between 0 and 0.5
}

float AudioEffectGesture::calculateRate(float input) {
  return map(input * RATE_RESOLUTION, -RATE_RESOLUTION, RATE_RESOLUTION, MIN_RATE, MAX_RATE);
}