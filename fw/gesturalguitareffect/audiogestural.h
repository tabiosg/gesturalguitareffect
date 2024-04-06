#include <Audio.h>
#include "guitareffects.h"

#define DELAY_LENGTH 44100

#define MAX_NUMBER_DELAY_REPEATS 10  // Needs to be 2 or greater
#define MIN_NUMER_DELAY_REPEATS 2
#define MAX_DELAY_STEP_SIZE (DELAY_LENGTH / MAX_NUMBER_DELAY_REPEATS)

// #define DEBUG

#define MAX_RATE 15
#define MIN_RATE 1

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
    #ifdef DEBUG
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
    #endif
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

  // ARM DSP Math library filter instance
  arm_biquad_cascade_df2T_instance_f32 iir_inst;
  float StateF32[1 * 4];
};