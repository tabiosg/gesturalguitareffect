#include "audiogestural.h"

void AudioEffectGesture::peakingCoefficients(float G, float fc, float Q, float fs) {

    float K = tan(M_PI * fc / fs);
    float V0 = pow(10, G / 20);
    float b0, b1, b2, a1, a2;
    if (G >= 0) {
      b0 = (1 + ((V0 / Q) * K) + pow(K, 2)) / (1 + ((1 / Q) * K) + pow(K, 2));
      b1 = (2 * (pow(K, 2) - 1)) / (1 + ((1 / Q) * K) + pow(K, 2));
      b2 = (1 - ((V0 / Q) * K) + pow(K, 2)) / (1 + ((1 / Q) * K) + pow(K, 2));
      a1 = b1;
      a2 = (1 - ((1 / Q) * K) + pow(K, 2)) / (1 + ((1 / Q) * K) + pow(K, 2));
    }
    else {
      b0 = (1 + ((1 / Q) * K) + pow(K, 2)) / (1 + ((V0 / Q) * K) + pow(K, 2));
      b1 = (2 * (pow(K, 2) - 1)) / (1 + ((V0 / Q) * K) + pow(K, 2));
      b2 = (1 - ((1 / Q) * K) + pow(K, 2)) / (1 + ((V0 / Q) * K) + pow(K, 2));
      a1 = b1;
      a2 = (1 - ((V0 / Q) * K) + pow(K, 2)) / (1 + ((V0 / Q) * K) + pow(K, 2));
    }

    // Assign the coefficients to the provided arrays
    __disable_irq();
    b[0] = b0;
    b[1] = b1;
    b[2] = b2;
    a[0] = 1.0f;
    a[1] = a1;
    a[2] = a2;
    __enable_irq();
}

void AudioEffectGesture::applyPeakingCoefficients() {
  peakingCoefficients(mCurrentGain, mCurrentCenterFrequency, 1.0f, 44100.0f);
}

void AudioEffectGesture::applyBiquad(float32_t *input, float32_t *output, uint32_t blockSize) {
    static float32_t x1 = 0, x2 = 0; // Delay elements
    static float32_t y1 = 0, y2 = 0; // Delay elements
    
    float32_t b0, b1, b2, a0, a1, a2;
    
    b0 = b[0]; // Coefficients
    b1 = b[1];
    b2 = b[2];
    a0 = a[0];
    a1 = a[1];
    a2 = a[2];

    for (uint32_t i = 0; i < blockSize; i++) {
        // Calculate output using difference equation
        float32_t y0 = (b0 / a0) * input[i] + (b1 / a0) * x1 + (b2 / a0) * x2 - (a1 / a0) * y1 - (a2 / a0) * y2;
        
        // Update delay elements
        x2 = x1;
        x1 = input[i];
        y2 = y1;
        y1 = y0;
        
        // Store filtered output
        output[i] = y0;  // Multiply by 0.8 to prevent going too high
    }
}
