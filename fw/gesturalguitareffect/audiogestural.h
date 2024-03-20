#include <Audio.h>
#include "guitareffects.h"

#define DELAY_LENGTH 44100

#define MAX_NUMBER_DELAY_REPEATS 5  // Needs to be 2 or greater
#define MAX_DELAY_STEP_SIZE (DELAY_LENGTH / MAX_NUMBER_DELAY_REPEATS)

class AudioEffectGesture : public AudioStream {
public:

  AudioEffectGesture();

  void changeEffect(GuitarEffect effect);

  virtual void update(void);

  void updatePotentiometer(float value);

  void updateAccelerometer(float value);

  GuitarEffect getEffect() {
    return mCurrentEffect;
  }

  void printEffect() {
    switch (mCurrentEffect) {
        case GuitarEffect::None:
            Serial.println("$EFFECT, None");
            break;
        case GuitarEffect::Tremolo:
            Serial.println("$EFFECT, Tremolo");
            break;
        case GuitarEffect::Delay:
            Serial.println("$EFFECT, Delay");
            break;
        case GuitarEffect::Fuzz:
            Serial.println("$EFFECT, Fuzz");
            break;
        default:
            Serial.println("Unknown effect");
    }
  }

private:
  void applyDelay(audio_block_t *block);

  void updateTremolo(audio_block_t *block);

  void updateDelayBuffer(audio_block_t *block);

  GuitarEffect mCurrentEffect;
  audio_block_t *inputQueueArray[1];
  int16_t mDelayBuffer[DELAY_LENGTH];
  uint32_t mWriteIndex;
  int mCurrentNumberDelayRepeats;
  int mCurrentDelayStepSize;
  float mDelayRatios[MAX_NUMBER_DELAY_REPEATS];
  float mDepth; 
  float mRate;
};