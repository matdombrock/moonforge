#pragma once
#include <portaudio.h>

int as_synthesis_callback(const void *inputBuffer, void *outputBuffer,
                 unsigned long framesPerBuffer,
                 const PaStreamCallbackTimeInfo *timeInfo,
                 PaStreamCallbackFlags statusFlags, void *userData);

int as_init();
int as_sleep(int ms);
int as_close();

