#pragma once
typedef struct {
    float *buffer;
    int size;
    int index;
    float feedback; // 0.0 to <1.0
    float mix;      // 0.0 (dry) to 1.0 (wet)
} mfx_delay;

void mfx_delay_init(mfx_delay *delay, int delay_samples, float feedback, float mix);
void mfx_delay_free(mfx_delay *delay);
void mfx_delay_set(mfx_delay *delay, int delay_samples, float feedback, float mix);
float mfx_delay_process(mfx_delay *delay, float x);
