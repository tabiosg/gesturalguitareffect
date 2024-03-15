#include <Audio.h>
#include "guitareffects.h"

#define DELAY_LENGTH 44100

#define NUMBER_DELAY_REPEATS 10  // Needs to be 2 or greater

class AudioEffectGesture : public AudioStream {
public:

  AudioEffectGesture();

  void changeEffect(GuitarEffect effect);

  virtual void update(void);

private:
  void applyDelay(audio_block_t *block);

  void updateDelayBuffer(audio_block_t *block);

  GuitarEffect mCurrentEffect;
  audio_block_t *inputQueueArray[1];
  int16_t mDelayBuffer[DELAY_LENGTH];
  uint32_t mWriteIndex;
  float mDelayRatios[NUMBER_DELAY_REPEATS];
};