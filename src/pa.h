#pragma once
#include <stdio.h>
#include <math.h>
#include <portaudio.h>
#include <pthread.h>

#include "config.h"
#include "util.h"
#include "synth.h"

// Forward declarations to avoid circular dependencies
extern pthread_mutex_t vis_mutex;

// Audio thread control
pthread_t audio_thread_id;
volatile int audio_thread_running = 0;
PaStream *audio_stream = NULL;
Synth_Internal audio_data;

// Declare functions first, then implement them below
static int pa_callback(const void *input_buffer, void *output_buffer,
        unsigned long buffer_size,
        const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags,
        void *user_data);
static void* pa_thread_func(void* arg);
int pa_start_thread();
void pa_stop_thread();
int pa_init();

// Audio callback function 
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

// Audio thread function
static void* pa_thread_func(void* arg) {
    PaError err;
    
    debug("pa: init on secondary thread\n");
    err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        return NULL;
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
    err = Pa_OpenDefaultStream(&audio_stream,
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
        return NULL;
    }

    debug("pa: start stream\n");
    err = Pa_StartStream(audio_stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
        Pa_CloseStream(audio_stream);
        Pa_Terminate();
        return NULL;
    }
    
    printf("------- AUDIO SYSTEM STARTED ON SECONDARY THREAD -------\n");
    
    // Keep the thread running until signaled to stop
    while (audio_thread_running) {
        Pa_Sleep(100); // Sleep to avoid hogging CPU
    }
    
    // Cleanup when thread is signaled to stop
    err = Pa_StopStream(audio_stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    }

    err = Pa_CloseStream(audio_stream);
    if (err != paNoError) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(err));
    }
    
    Pa_Terminate();
    printf("Audio thread terminated\n");
    return NULL;
}

// Initialize and start the audio thread
int pa_start_thread() {
    // Initialize audio data
    for (int i = 0; i < OSC_COUNT; i++) {
        audio_data.phase[i] = 0.0f;
    }
    
    // Initialize Lua state at startup
    luaB_init();
    
    // Set the flag to indicate the thread should run
    audio_thread_running = 1;
    
    // Create and start the audio thread
    int result = pthread_create(&audio_thread_id, NULL, pa_thread_func, NULL);
    if (result != 0) {
        fprintf(stderr, "Error creating audio thread: %d\n", result);
        return 1;
    }
    
    return 0;
}

// Stop the audio thread and wait for it to finish
void pa_stop_thread() {
    if (audio_thread_running) {
        printf("Stopping audio thread...\n");
        audio_thread_running = 0;
        
        // Wait for the thread to finish
        pthread_join(audio_thread_id, NULL);
        
        // Clean up Lua state
        luaB_cleanup();
        
        printf("Audio thread stopped\n");
    }
}

// Legacy function for backward compatibility, not used in new architecture
int pa_init() {
    fprintf(stderr, "Warning: pa_init() is deprecated. Use pa_start_thread() instead.\n");
    return 1;
}
