// Recording module for audio data
#include "const.h"
#include <stdint.h>
#include <time.h>

// A buffer to hold the audio data for recording.
int recording_index = 0; // Current index in the recording buffer
#if RECORDING_ENABLED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
float recording_buffer[RECORDING_BUFFER_SIZE];

// Takes a raw file name without timestamp or extension
// and returns a dynamically allocated string with the full file name
char *rec_make_filename(const char *filename) {
  size_t len = strlen(filename) + 1 + 64 + 4 + 1;
  char *filename_full = malloc(len);
  if (!filename_full)
    return NULL;

  strcpy(filename_full, filename);
  strcat(filename_full, "_");

  char timestamp[64];
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H:%M:%S", tm_info);
  strcat(filename_full, timestamp);

  strcat(filename_full, ".wav");
  return filename_full;
}

// Writes stereo buffer data to the specified WAV file.
void rec_write_wav(const char *filename, float *data, int num_frames, int sample_rate) {
  if (filename == NULL || data == NULL || num_frames <= 0 || sample_rate <= 0) {
    fprintf(stderr, "Invalid parameters for writing WAV file.\n");
    return;
  }
  FILE *f = fopen(filename, "wb");
  int num_channels = 2;
  int bits_per_sample = 32;
  int byte_rate = sample_rate * num_channels * bits_per_sample / 8;
  int block_align = num_channels * bits_per_sample / 8;
  int data_size = num_frames * num_channels * sizeof(float);

  // Write RIFF header
  fwrite("RIFF", 1, 4, f);
  uint32_t chunk_size = 36 + data_size;
  fwrite(&chunk_size, 4, 1, f);
  fwrite("WAVE", 1, 4, f);

  // Write fmt chunk
  fwrite("fmt ", 1, 4, f);
  uint32_t fmt_size = 16;
  fwrite(&fmt_size, 4, 1, f);
  uint16_t audio_format = 3; // IEEE float
  fwrite(&audio_format, 2, 1, f);
  fwrite(&num_channels, 2, 1, f);
  fwrite(&sample_rate, 4, 1, f);
  fwrite(&byte_rate, 4, 1, f);
  fwrite(&block_align, 2, 1, f);
  fwrite(&bits_per_sample, 2, 1, f);

  // Write data chunk
  fwrite("data", 1, 4, f);
  fwrite(&data_size, 4, 1, f);
  fwrite(data, sizeof(float), num_frames * num_channels, f);

  float seconds = (float)num_frames / sample_rate;
  fclose(f);
  printf(COL_YELLOW);
  printf("///////\n");
  printf("//WAV file written\n");
  printf("//%s\n", filename);
  printf("//%fs\n", seconds);
  printf("//%d frames at %dHz.\n", num_frames, sample_rate);
  printf("///////\n");
  printf(COL_RESET);
}

// Initializes the recording buffer and sets the index to 0.
void rec_init() {
  // Initialize the recording buffer
  recording_index = 0;
  for (int i = 0; i < RECORDING_BUFFER_SIZE; i++) {
    recording_buffer[i] = 0.0f; // Clear the buffer
  }
}

// Wrapper on rec_write_wav that writes the current recording buffer to a WAV file and resets the index.
// NOTE: This could be changed to write another type of file in the future.
int rec_write_recording() {
  float *data = recording_buffer;
  float num_frames = (int)(recording_index / 2); // Number of frames is half the buffer size (stereo)
  char *filename = "mf";
  filename = rec_make_filename(filename);
  rec_write_wav(filename, data, num_frames, SAMPLE_RATE);
  recording_index = 0;
  return 0; // Success
}

// Writes a pair of stereo audio samples to the recording buffer.
void rec_write_sample(float sample_l, float sample_r) {
  // Record audio samples into the buffer
  if (recording_index < RECORDING_BUFFER_SIZE) {
    recording_buffer[recording_index] = sample_l; // Left channel
    recording_index++;
    recording_buffer[recording_index] = sample_r; // Right channel
    recording_index++;
  }
  if (recording_index >= RECORDING_BUFFER_SIZE) {
    rec_write_recording();
  }
}
#else
float recording_buffer[1]; // Dummy buffer if recording is disabled

char *rec_make_filename(const char *filename) {
  // No recording logic if RECORDING_ENABLED is 0
  return NULL;
}

void rec_write_wav(const char *filename, float *data, int num_frames, int sample_rate) {
  // No recording logic if RECORDING_ENABLED is 0
}

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
