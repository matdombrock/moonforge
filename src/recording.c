#include "const.h"
// A buffer to hold the audio data for recording.
int recording_index = 0; // Current index in the recording buffer
#if RECORDING_ENABLED
#include "util.h"
float recording_buffer[RECORDING_BUFFER_SIZE];
void rec_init() {
  // Initialize the recording buffer
  recording_index = 0;
  for (int i = 0; i < RECORDING_BUFFER_SIZE; i++) {
    recording_buffer[i] = 0.0f; // Clear the buffer
  }
}
int rec_write_recording() {
  float *data = recording_buffer;
  float num_frames = (int)(recording_index / 2); // Number of frames is half the buffer size
  char *filename = "mf";
  filename = util_make_recording_filename(filename);
  util_write_wav(filename, data, num_frames, SAMPLE_RATE);
  recording_index = 0;
  return 0; // Success
}
void rec_write_sample(float sample_l, float sample_r) {
  // Record audio samples into the buffer
  if (recording_index < RECORDING_BUFFER_SIZE - 2) {
    recording_buffer[recording_index++] = sample_l; // Left channel
    recording_buffer[recording_index++] = sample_r; // Right channel
  } else {
    // Write the current buffer to file
    // recording_index is reset to 0 after writing
    rec_write_recording();
  }
}
#else
float recording_buffer[1]; // Dummy buffer if recording is disabled
void rec_init() {
  // No recording logic if RECORDING_ENABLED is 0
}
int rec_write_recording() {
  // No recording logic if RECORDING_ENABLED is 0
  return 0; // Success
}
void rec_write_sample(float sample_l, float sample_r) {
  // No recording logic if RECORDING_ENABLED is 0
}
#endif
