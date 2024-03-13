#include "audiogestural.h"

AudioEffectGesture::AudioEffectGesture() : AudioStream(1, inputQueueArray) {
  // mCurrentEffect = GuitarEffect::None;
  mCurrentEffect = GuitarEffect::Delay;
  memset(mDelayBuffer, 0, DELAY_LENGTH * sizeof(int16_t));
  mWriteIndex = 0;
  mDelayMixRatio = 0.5; // Initial mix ratio (50% delayed, 50% original)
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
      // Pass the input data to the output
      transmit(block);
      // Release the input data block
      release(block);
    }
  }
  else if (mCurrentEffect == GuitarEffect::Delay) {
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