#pragma once

char *util_get_args(int argc, char *argv[]);
void util_print_startup_info(char *script_path);
void util_write_wav(const char *filename, float *data, int num_frames, int sample_rate);
