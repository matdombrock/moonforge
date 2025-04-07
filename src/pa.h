#pragma once
#include <stdio.h>
#include <math.h>
#include <portaudio.h>
#include <pthread.h>

#include "config.h"
#include "util.h"
#include "synth.h"

// Audio thread control
static pthread_t audio_thread_id;
static volatile int audio_thread_running = 0;
static PaStream *stream = NULL;
static Synth_Internal audio_data;

// Audio callback function called by PortAudio
static int pa_callback(const void *input_buffer, void *output_buffer,
        unsigned long buffer_size,
        const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags,
        void *user_data)
{
    Synth_Internal *data = (Synth_Internal *)user_data;
    float *out = (float *)output_buffer;
    // Run Lua
    synth_lua(); 
    // Generate samples
    synth_get_buffer(data, out); 
    return paContinue;
}

// Initialize PortAudio and create a stream
static int pa_init_stream() {
    PaError err;
    
    // Initialize data
    for (int i = 0; i < OSC_COUNT; i++) {
        audio_data.phase[i] = 0.0f;
    }
    
    debug("pa: init\n");
    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return 1;
    }
    
    // Set PortAudio host API for macOS
    #ifdef __APPLE__
    PaHostApiIndex hostApiIndex = Pa_HostApiTypeIdToHostApiIndex(paCoreAudio);
    if (hostApiIndex != paHostApiNotFound) {
        const PaHostApiInfo *hostApiInfo = Pa_GetHostApiInfo(hostApiIndex);
        if (hostApiInfo && hostApiInfo->defaultOutputDevice != paNoDevice) {
            // Get device info to optimize buffer settings
            const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(hostApiInfo->defaultOutputDevice);
            if (deviceInfo) {
                debug("pa: using CoreAudio with optimal settings\n");
            }
        }
    }
    #endif
    
    debug("pa: open default stream\n");
    err = Pa_OpenDefaultStream(&stream,
        0,          // input channels
        2,          // output channels
        paFloat32,  // 32-bit floating point output
        (float)SAMPLE_RATE / DOWNSAMPLE,
        BUFFER_SIZE,
        pa_callback,
        &audio_data);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_Terminate();
        return 1;
    }
    
    return 0;
}

// Start the audio stream
static int pa_start_stream() {
    PaError err;
    
    debug("pa: start stream\n");
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }
    
    printf("------- AUDIO SYSTEM STARTED -------\n");
    char modes[][32] = {"none", "debug", "visualizer"};
    printf("Console output mode: %s\n", modes[_sys.output_mode]);
    
    return 0;
}

// Stop the audio stream
static int pa_stop_stream() {
    PaError err;
    
    if (stream) {
        err = Pa_StopStream(stream);
        if (err != paNoError) {
            fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        }
        
        err = Pa_CloseStream(stream);
        if (err != paNoError) {
            fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        }
        
        stream = NULL;
    }
    
    Pa_Terminate();
    return 0;
}

// Audio thread function
static void* pa_thread_func(void* arg) {
    // Initialize Lua state at startup
    luaB_init();
    
    // Initialize and start PortAudio stream
    if (pa_init_stream() != 0) {
        fprintf(stderr, "Failed to initialize PortAudio stream\n");
        return NULL;
    }
    
    if (pa_start_stream() != 0) {
        fprintf(stderr, "Failed to start PortAudio stream\n");
        return NULL;
    }
    
    // Keep the thread running until asked to stop
    while (audio_thread_running) {
        Pa_Sleep(100); // Sleep to reduce CPU usage
    }
    
    // Clean up when thread is stopped
    pa_stop_stream();
    
    // Clean up Lua state
    luaB_cleanup();
    
    return NULL;
}

// Start the audio processing in a separate thread
int pa_start_thread() {
    audio_thread_running = 1;
    
    int result = pthread_create(&audio_thread_id, NULL, pa_thread_func, NULL);
    if (result != 0) {
        fprintf(stderr, "Error creating audio thread: %d\n", result);
        return 1;
    }
    
    return 0;
}

// Stop the audio thread
void pa_stop_thread() {
    if (audio_thread_running) {
        audio_thread_running = 0;
        
        // Wait for the thread to finish
        pthread_join(audio_thread_id, NULL);
    }
}
