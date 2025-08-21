#include <stdlib.h>
#include <string.h>
#include "delay.h"

void mfx_delay_init(mfx_delay *delay, int delay_samples, float feedback, float mix) {
    delay->buffer = (float*)malloc(sizeof(float) * delay_samples);
    memset(delay->buffer, 0, sizeof(float) * delay_samples);
    delay->size = delay_samples;
    delay->index = 0;
    delay->feedback = feedback;
    delay->mix = mix;
}

void mfx_delay_free(mfx_delay *delay) {
    free(delay->buffer);
    delay->buffer = NULL;
}

void mfx_delay_set(mfx_delay *delay, int delay_samples, float feedback, float mix) {
    float *new_buffer = (float*)realloc(delay->buffer, sizeof(float) * delay_samples);
    if (new_buffer) {
        delay->buffer = new_buffer;
        delay->size = delay_samples;
    }
    delay->feedback = feedback;
    delay->mix = mix;
    // Do not reset buffer or index
}

float mfx_delay_process(mfx_delay *delay, float x) {
    float delayed = delay->buffer[delay->index];
    float input = x + delayed * delay->feedback;
    delay->buffer[delay->index] = input;
    delay->index = (delay->index + 1) % delay->size;
    return x * (1.0f - delay->mix) + delayed * delay->mix;
}

