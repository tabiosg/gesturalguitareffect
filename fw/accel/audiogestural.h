#include <Audio.h>
#include "guitareffects.h"

class AudioEffectGesture : public AudioStream {
public:

  AudioEffectGesture();

  void changeEffect(GuitarEffect effect);

  virtual void update(void);

private:
  GuitarEffect mCurrentEffect;
  audio_block_t *inputQueueArray[1];
};