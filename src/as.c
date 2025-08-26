// This file contains the PortAudio wrapper functions for audio playback.
// In thoery this is all that would need to be replaced to use a different
// audio library, such as SDL, OpenAL, JUCE or a custom audio system.

#include "as.h"
#include "const.h"
#include "mf.h"

// Main function handling the audio synthesis callback.
// time_info and user_data are not used here
// no actual PortAudio code is used here
int as_synthesis_callback(const void *input_buffer, void *output_buffer, unsigned long frames_per_buffer,
                          const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags,
                          void *user_data) {

  if (state.flags.exit) {
    return paComplete; // Exit if the exit flag is set
  }
  mf_synth_callback(output_buffer, frames_per_buffer, user_data);
  return paContinue;
}

PaStream *stream;
int as_init() {
  Pa_Initialize();
  Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, SAMPLE_RATE, BLOCK_SIZE, as_synthesis_callback, &wave_data);
  Pa_StartStream(stream);
  return 0;
}

int as_sleep(int ms) {
  Pa_Sleep(ms);
  return 0; // Success
}

int as_close() {
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();
  return 0; // Success
}
