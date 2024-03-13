#include <Audio.h>
#include "guitareffects.h"

#define DELAY_LENGTH 44100

class AudioEffectGesture : public AudioStream {
public:

  AudioEffectGesture();

  void changeEffect(GuitarEffect effect);

  virtual void update(void);

private:
  void applyDelay(audio_block_t *block) {
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
      int16_t input = block->data[i];
      int16_t output = mDelayBuffer[mWriteIndex];
      mDelayBuffer[mWriteIndex] = input;
      mWriteIndex = (mWriteIndex + 1) % DELAY_LENGTH;
      block->data[i] = output;
    }
    // Output the delayed audio
    transmit(block);
  }

  GuitarEffect mCurrentEffect;
  audio_block_t *inputQueueArray[1];
  int16_t mDelayBuffer[DELAY_LENGTH];
  uint32_t mWriteIndex;
};