#pragma once
typedef struct {
    float *buffer;
    int size;
    int index;
    float feedback; // 0.0 to <1.0
    float mix;      // 0.0 (dry) to 1.0 (wet)
} Delay;

void delay_init(Delay *delay, int delay_samples, float feedback, float mix);
void delay_free(Delay *delay);
void delay_set(Delay *delay, int delay_samples, float feedback, float mix);
float delay_process(Delay *delay, float x);
