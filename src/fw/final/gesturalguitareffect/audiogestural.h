#include <Audio.h>
#include "guitareffects.h"

#define DELAY_LENGTH 88200

#define MAX_NUMBER_DELAY_REPEATS 10  // Needs to be 2 or greater
#define MIN_NUMER_DELAY_REPEATS 2
#define MAX_DELAY_STEP_SIZE (DELAY_LENGTH / (mCurrentNumberDelayRepeats - 1))
#define MIN_DELAY_STEP_SIZE 4000

#define MIN_CENTER_FREQUENCY 100
#define MAX_CENTER_FREQUENCY 7000
#define CENTER_FREQUENCY_RESOLUTION 10
#define GAIN_RESOLUTION 10
#define MIN_GAIN -10
#define MAX_GAIN 20

#define REPEATS_RESOLUTION 5
#define STEP_SIZE_RESOLUTION 5

#define MIN_RATE 1
#define MAX_RATE 15
#define RATE_RESOLUTION 10
#define DEPTH_RESOLUTION 10

#define ACCEL_MIN_DEG_FOR_INPUT -20
#define ACCEL_MAX_DEG_FOR_INPUT 30 // Degrees since we noticed that anything above 50 degrees is not too accurate. -- but 50 degrees is a lot to move
#define ACCEL_DEG_THRESHOLDING 1

#define ACCEL_CALLS_TO_WAIT_BEFORE_UPDATING_AGAIN 3

// #define DEBUG

class AudioEffectGesture : public AudioStream {
public:

  AudioEffectGesture();

  void changeEffect(GuitarEffect effect);

  virtual void update(void);

  void updatePotentiometer(float value);

  void updateAccelerometer(float value);

  void applyPeakingCoefficients();

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
        case GuitarEffect::Wah:
            Serial.println("$EFFECT, Wah");
            break;
        default:
            Serial.println("Unknown effect");
    }
    #endif
  }

private:
  void applyDelay(audio_block_t *block);

  void updateTremolo(audio_block_t *block, int startingIndex);

  void updateDelayBuffer(audio_block_t *block);

  void peakingCoefficients(float G, float fc, float Q, float fs);

  void applyBiquad(float32_t *input, float32_t *output, uint32_t blockSize);

  int calculateRepeats(float input);

  int calculateStepSize(float input);

  float calculateDepth(float input);

  float calculateRate(float input);

  int calculateGain(float input);

  int calculateCenterFrequency(float input);

  float convertAccelValueToInput(float value);

  GuitarEffect mCurrentEffect;
  audio_block_t *inputQueueArray[1];
  int16_t mDelayBuffer[DELAY_LENGTH];
  uint32_t mWriteIndex;
  int mCurrentNumberDelayRepeats;
  int mCurrentDelayStepSize;
  float mDelayRatios[MAX_NUMBER_DELAY_REPEATS];
  float mDepth; 
  float mRate;

  float mCurrentCenterFrequency;
  float mCurrentGain; 

  float32_t mb0, mb1, mb2, ma1, ma2;
};