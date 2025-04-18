#pragma once
#include <string.h>
#include <stdlib.h>
typedef struct {
    char name[32];
    float freq;
} Note;

Note notes[] = {
    {"C0", 16.35},
    {"C#0", 17.32},
    {"D0", 18.35},
    {"D#0", 19.45},
    {"E0", 20.60},
    {"F0", 21.83},
    {"F#0", 23.12},
    {"G0", 24.50},
    {"G#0", 25.96},
    {"A0", 27.50},
    {"A#0", 29.14},
    {"B0", 30.87},
    {"C1", 32.70},
    {"C#1", 34.65},
    {"D1", 36.71},
    {"D#1", 38.89},
    {"E1", 41.20},
    {"F1", 43.65},
    {"F#1", 46.25},
    {"G1", 49.00},
    {"G#1", 51.91},
    {"A1", 55.00},
    {"A#1", 58.27},
    {"B1", 61.74},
    {"C2", 65.41},
    {"C#2", 69.30},
    {"D2", 73.42},
    {"D#2", 77.78},
    {"E2", 82.41},
    {"F2", 87.31},
    {"F#2", 92.50},
    {"G2", 98.00},
    {"G#2", 103.83},
    {"A2", 110.00},
    {"A#2", 116.54},
    {"B2", 123.47},
    {"C3", 130.81},
    {"C#3", 138.59},
    {"D3", 146.83},
    {"D#3", 155.56},
    {"E3", 164.81},
    {"F3", 174.61},
    {"F#3", 185.00},
    {"G3", 196.00},
    {"G#3", 207.65},
    {"A3", 220.00},
    {"A#3", 233.08},
    {"B3", 246.94},
    {"C4", 261.63},
    {"C#4", 277.18},
    {"D4", 293.66},
    {"D#4", 311.13},
    {"E4", 329.63},
    {"F4", 349.23},
    {"F#4", 369.99},
    {"G4", 392.00},
    {"G#4", 415.30},
    {"A4", 440.00},
    {"A#4", 466.16},
    {"B4", 493.88},
    {"C5", 523.25},
    {"C#5", 554.37},
    {"D5", 587.33},
    {"D#5", 622.25},
    {"E5", 659.25},
    {"F5", 698.46},
    {"F#5", 739.99},
    {"G5", 783.99},
    {"G#5", 830.61},
    {"A5", 880.00},
    {"A#5", 932.33},
    {"B5", 987.77},
    {"C6", 1046.50},
    {"C#6", 1108.73},
    {"D6", 1174.66},
    {"D#6", 1244.51},
    {"E6", 1318.51},
    {"F6", 1396.91},
    {"F#6", 1479.98},
    {"G6", 1567.98},
    {"G#6", 1661.22},
    {"A6", 1760.00},
    {"A#6", 1864.66},
    {"B6", 1975.53},
    {"C7", 2093.00},
    {"C#7", 2217.46},
    {"D7", 2349.32},
    {"D#7", 2489.02},
    {"E7", 2637.02},
    {"F7", 2793.83},
    {"F#7", 2959.96},
    {"G7", 3135.96},
    {"G#7", 3322.44},
    {"A7", 3520.00},
    {"A#7", 3729.31},
    {"B7", 3951.07},
    {"C8", 4186.01},
    {"C#8", 4434.92},
    {"D8", 4698.63},
    {"D#8", 4978.03},
    {"E8", 5274.04},
    {"F8", 5587.65},
    {"F#8", 5919.91},
    {"G8", 6271.93},
    {"G#8", 6644.88},
    {"A8", 7040.00},
    {"A#8", 7458.62},
    {"B8", 7902.13},
    {"C9", 8372.02},
    {"C#9", 8869.84},
    {"D9", 9397.27},
    {"D#9", 9956.06},
    {"E9", 10548.08},
    {"F9", 11175.30},
    {"F#9", 11839.82},
    {"G9", 12543.86},
    {"G#9", 13289.75},
    {"A9", 14080.00},
    {"A#9", 14917.24},
    {"B9", 15804.27},
    {"C10", 16744.04},
    {"C#10", 17739.68},
    {"D10", 18794.55},
    {"D#10", 19912.12},
    {"E10", 21096.16},
    {"F10", 22350.60},
    {"F#10", 23679.64},
    {"G10", 25087.72},
    {"G#10", 26579.50},
    {"A10", 28160.00},
    {"A#10", 29834.48},
    {"B10", 31608.54},
    {"C11", 33488.08},
    {"C#11", 35479.36},
    {"D11", 37589.10},
    {"D#11", 39824.24},
    {"E11", 42192.32},
    {"F11", 44701.20},
    {"F#11", 47359.28},
    {"G11", 50175.44},
    {"G#11", 53159.00},
    {"A11", 56320.00},
    {"A#11", 59668.96},
    {"B11", 63217.08},
    {"C12", 66976.16},
    {"C#12", 70958.72},
    {"D12", 75178.20},
    {"D#12", 79648.48},
    {"E12", 84384.64},
    {"F12", 89402.40},
    {"F#12", 94718.56},
    {"G12", 100351.00},
    {"G#12", 106318.00},
    {"A12", 112640.00},
    {"A#12", 119337.92},
    {"B12", 126434.16},
    {"C13", 133952.32},
    {"C#13", 141917.44},
    {"D13", 150356.40},
    {"D#13", 159296.96},
    {"E13", 168769.28},
    {"F13", 178804.80},
    {"F#13", 189437.12},
    {"G13", 200702.00},
    {"G#13", 212636.00},
    {"A13", 225280.00},
    {"A#13", 238675.84},
    {"B13", 252868.32},
    {"C14", 267904.64},
    {"C#14", 283834.88},
    {"D14", 300712.80},
    {"D#14", 318593.92},
    {"E14", 337538.56},
    {"F14", 357609.60},
    {"F#14", 378874.24},
    {"G14", 401404.00},
    {"G#14", 425272.00},
    {"A14", 450560.00},
    {"A#14", 477351.68},
    {"B14", 505736.64},
    {"C15", 535809.28},
    {"C#15", 567669.76},
    {"D15", 601425.60},
    {"D#15", 637187.84},
    {"E15", 675077.12},
    {"F15", 715219.20},
    {"F#15", 757748.48},
    {"G15", 802808.00},
    {"G#15", 850544.00},
    {"A15", 901120.00},
    {"A#15", 954703.36},
    {"B15", 1011473.28},
    {"C16", 1071618.56},
    {"C#16", 1135339.52},
    {"D16", 1202851.20},
    {"D#16", 1274375.68},
    {"E16", 1350154.24},
    {"F16", 1430438.40},
    {"F#16", 1515496.96},
    {"G16", 1605616.00},
    {"G#16", 1701088.00},
    {"A16", 1802240.00},
    {"A#16", 1909406.72},
    {"B16", 2022946.56},
    {"C17", 2143237.12},
};

Note note_by_name(const char *name) {
    for (int i = 0; i < sizeof(notes) / sizeof(Note); i++) {
        if (strcmp(notes[i].name, name) == 0) {
            return notes[i];
        }
    }
    return notes[0];
}

// A function that takes a frequency and returns the closest note
Note note_by_freq(float freq) {
    Note closest_note = notes[0];
    float closest_diff = 1000000;
    for (int i = 0; i < sizeof(notes) / sizeof(Note); i++) {
        float diff = abs(notes[i].freq - freq);
        if (diff < closest_diff) {
            closest_diff = diff;
            closest_note = notes[i];
        }
    }
    return closest_note;
}
