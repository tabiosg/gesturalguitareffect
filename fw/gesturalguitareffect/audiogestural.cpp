#include "audiogestural.h"

AudioEffectGesture::AudioEffectGesture() : AudioStream(1, inputQueueArray) {
  mCurrentEffect = GuitarEffect::None;
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
}