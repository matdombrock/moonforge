#pragma once

typedef struct {
    float a;      // smoothing factor
    float y_prev; // previous output sample
    float cutoff; // cutoff frequency cached val
} mfx_lowpass_state;

void mfx_lowpass_init(mfx_lowpass_state *filter, float cutoff);
void mfx_lowpass_set(mfx_lowpass_state *filter, float cutoff);
float mfx_lowpass_process(mfx_lowpass_state *filter, float x);
