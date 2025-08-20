#include <stdlib.h>
#include <string.h>
#include "const.h"
#include "delay.h"

void delay_init(Delay *delay, int delay_samples, float feedback, float mix) {
    delay->buffer = (float*)malloc(sizeof(float) * delay_samples);
    memset(delay->buffer, 0, sizeof(float) * delay_samples);
    delay->size = delay_samples;
    delay->index = 0;
    delay->feedback = feedback;
    delay->mix = mix;
}

void delay_free(Delay *delay) {
    free(delay->buffer);
    delay->buffer = NULL;
}

void delay_set(Delay *delay, int delay_samples, float feedback, float mix) {
    float *new_buffer = (float*)realloc(delay->buffer, sizeof(float) * delay_samples);
    if (new_buffer) {
        delay->buffer = new_buffer;
        delay->size = delay_samples;
    }
    delay->feedback = feedback;
    delay->mix = mix;
    // Do not reset buffer or index
}

float delay_process(Delay *delay, float x) {
    float delayed = delay->buffer[delay->index];
    float input = x + delayed * delay->feedback;
    delay->buffer[delay->index] = input;
    delay->index = (delay->index + 1) % delay->size;
    return x * (1.0f - delay->mix) + delayed * delay->mix;
}

