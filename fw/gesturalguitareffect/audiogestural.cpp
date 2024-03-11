#include "audiogestural.h"

AudioEffectGesture::AudioEffectGesture() : AudioStream(1, inputQueueArray) {

}

void AudioEffectGesture::changeEffect(GuitarEffect effect) {
  mCurrentEffect = effect;
}

void AudioEffectGesture::update(void) {
  
}