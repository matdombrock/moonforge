#pragma once

typedef struct {
    float cutoff; // cutoff frequency cached val
    float b0, b1, b2; // filter numerator coefficients
    float a1, a2;     // filter denominator coefficients
    float x1, x2;     // previous input samples
    float y1, y2;     // previous output samples
} mfx_lowpass_state;

void mfx_lowpass_init(mfx_lowpass_state *filter);
void mfx_lowpass_set(mfx_lowpass_state *filter, float cutoff);
float mfx_lowpass_process(mfx_lowpass_state *filter, float x);
