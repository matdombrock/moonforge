#pragma once
typedef struct {
    float *buffer;
    int window;    // delay in samples
    int size;
    int index;
    float feedback; // 0.0 to <1.0
    float mix;      // 0.0 (dry) to 1.0 (wet)
} mfx_delay_state;

void mfx_delay_init(mfx_delay_state *delay);
void mfx_delay_free(mfx_delay_state *delay);
void mfx_delay_set(mfx_delay_state *delay, int delay_samples, float feedback, float mix);
float mfx_delay_process(mfx_delay_state *delay, float x);
