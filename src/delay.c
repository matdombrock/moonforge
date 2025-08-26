#include "delay.h"
#include "const.h"
#include <stdlib.h>
#include <string.h>

// Returns a safe value for delay_samples
static int _mfx_delay_safe(int delay_samples) {
  if (delay_samples < 0) {
    return 1; // Negative delay not allowed
  }
  if (delay_samples > MAX_DELAY_SAMPLES) {
    return MAX_DELAY_SAMPLES; // Cap to max size
  }
  return delay_samples; // Valid
}

void mfx_delay_init(mfx_delay *delay) {
  delay->buffer = (float *)malloc(sizeof(float) * MAX_DELAY_SAMPLES);
  memset(delay->buffer, 0, sizeof(float) * MAX_DELAY_SAMPLES);
  delay->size = MAX_DELAY_SAMPLES;
  delay->index = 0;
  delay->feedback = 0.0f;
  delay->mix = 0.0f;
  delay->window = 1;
}

void mfx_delay_free(mfx_delay *delay) {
  free(delay->buffer);
  delay->buffer = NULL;
}

void mfx_delay_set(mfx_delay *delay, int delay_samples, float feedback, float mix) {
  delay->window = delay_samples;
  delay->feedback = feedback;
  delay->mix = mix;
  // Do not reset buffer or index
}

float mfx_delay_process(mfx_delay *delay, float x) {
  float delayed = 0.0f;
  // TODO: Is this really needed?
  if (delay->window > 0 && delay->window < delay->size) {
    int read_index = delay->index - delay->window;
    if (read_index < 0)
      read_index += delay->size;
    delayed = delay->buffer[read_index];
  }
  float input = x + delayed * delay->feedback;
  delay->buffer[delay->index] = input;
  delay->index = (delay->index + 1) % delay->size;
  return x * (1.0f - delay->mix) + delayed * delay->mix;
}
