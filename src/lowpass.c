#include "lowpass.h"
#include "const.h"
#include <math.h>

// Set the cutoff frequency of the second order lowpass filter
void mfx_lowpass_set(mfx_lowpass_state *filter, float cutoff) {
  filter->cutoff = cutoff; // Cache cutoff frequency
  float omega = 2.0f * M_PI * cutoff / SAMPLE_RATE;
  float cos_omega = cosf(omega);
  float sin_omega = sinf(omega);
  float Q = 0.7071f; // Butterworth (max flat) Q
  float alpha = sin_omega / (2.0f * Q);

  float b0 = (1.0f - cos_omega) / 2.0f;
  float b1 = 1.0f - cos_omega;
  float b2 = (1.0f - cos_omega) / 2.0f;
  float a0 = 1.0f + alpha;
  float a1 = -2.0f * cos_omega;
  float a2 = 1.0f - alpha;

  filter->b0 = b0 / a0;
  filter->b1 = b1 / a0;
  filter->b2 = b2 / a0;
  filter->a1 = a1 / a0;
  filter->a2 = a2 / a0;
}

// Initialize filter at default cutoff frequency
void mfx_lowpass_init(mfx_lowpass_state *filter) {
  filter->x1 = 0.0f;
  filter->x2 = 0.0f;
  filter->y1 = 0.0f;
  filter->y2 = 0.0f;
  mfx_lowpass_set(filter, 20000.0f);
}

// Process a single sample (Direct Form I)
float mfx_lowpass_process(mfx_lowpass_state *filter, float x) {
  float y = filter->b0 * x + filter->b1 * filter->x1 + filter->b2 * filter->x2
            - filter->a1 * filter->y1 - filter->a2 * filter->y2;
  filter->x2 = filter->x1;
  filter->x1 = x;
  filter->y2 = filter->y1;
  filter->y1 = y;
  return y;
}
