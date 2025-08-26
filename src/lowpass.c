#include "lowpass.h"
#include "const.h"
#include <math.h>

void mfx_lowpass_set(mfx_lowpass_state *filter, float cutoff) {
  filter->cutoff = cutoff; // Cache cutoff frequency
  float prev = filter->y_prev;
  float rc = 1.0f / (2.0f * M_PI * cutoff);
  float dt = 1.0f / SAMPLE_RATE;
  filter->a = dt / (rc + dt);
  // Preserve previous output sample
  filter->y_prev = prev;
}

// Initialize filter at cutoff frequency
void mfx_lowpass_init(mfx_lowpass_state *filter) {
  mfx_lowpass_set(filter, 20000.0f);
}

// Process a single sample
float mfx_lowpass_process(mfx_lowpass_state *filter, float x) {
  float y = filter->a * x + (1.0f - filter->a) * filter->y_prev;
  filter->y_prev = y;
  return y;
}
