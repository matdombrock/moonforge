#pragma once
#include <math.h>
#include "const.h"
#include "lowpass.h"

// Initialize filter at cutoff frequency
void lowpass_init(Lowpass_Filter *filter, float cutoff) {
    float rc = 1.0f / (2.0f * M_PI * cutoff);
    float dt = 1.0f / SAMPLE_RATE;
    filter->a = dt / (rc + dt);
    filter->y_prev = 0.0f;
}

void lowpass_set(Lowpass_Filter *filter, float cutoff) {
    // Reinitialize filter with new cutoff frequency
    float prev = filter->y_prev;
    lowpass_init(filter, cutoff);
    // Preserve previous output sample
    filter->y_prev = prev;
}

// Process a single sample
float lowpass_process(Lowpass_Filter *filter, float x) {
    float y = filter->a * x + (1.0f - filter->a) * filter->y_prev;
    filter->y_prev = y;
    return y;
}

