#pragma once

typedef struct {
    float a;      // smoothing factor
    float y_prev; // previous output sample
    float cutoff; // cutoff frequency cached val
} Lowpass_Filter;

void lowpass_init(Lowpass_Filter *filter, float cutoff);
void lowpass_set(Lowpass_Filter *filter, float cutoff);
float lowpass_process(Lowpass_Filter *filter, float x);
