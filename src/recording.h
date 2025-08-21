#pragma once

void rec_init();
void rec_write_sample(float sample_l, float sample_r);
int rec_write_recording();
char *rec_make_recording_filename(char *filename);
void rec_write_wav(const char *filename, float *data, int num_frames, int sample_rate);
