#include "audiogestural.h"

void AudioEffectGesture::updateTremolo(audio_block_t *block) {
  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float sincalc = sin(0.00014240362 * mWriteIndex * mRate); // sin(2 * M_PI * static_cast<float>(mWriteIndex) / DELAY_LENGTH * mRate);
    float factor = (1.0 - mDepth) + mDepth * sincalc;
    block->data[i] *= factor;
  }
}