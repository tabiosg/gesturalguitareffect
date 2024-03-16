#include <Audio.h>
#include "guitareffects.h"

#define DELAY_LENGTH 44100

#define MAX_NUMBER_DELAY_REPEATS 10  // Needs to be 2 or greater

class AudioEffectGesture : public AudioStream {
public:

  AudioEffectGesture();

  void changeEffect(GuitarEffect effect);

  virtual void update(void);

  void updatePotentiometer(float value);

  void updateAccelerometer(float value);

private:
  void applyDelay(audio_block_t *block);

  void updateTremolo(audio_block_t *block);

  void updateDelayBuffer(audio_block_t *block);

  void updateNumberDelayRepeats(int new_num);

  GuitarEffect mCurrentEffect;
  audio_block_t *inputQueueArray[1];
  int16_t mDelayBuffer[DELAY_LENGTH];
  uint32_t mWriteIndex;
  int mCurrentNumberDelayRepeats;
  int mCurrentDelayLength;
  float mDelayRatios[MAX_NUMBER_DELAY_REPEATS];
  float mDepth; 
  float mRate;
};