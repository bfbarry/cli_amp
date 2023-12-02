#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cmath>
#include <portaudio.h>
#include <iostream>
#include <signal.h>
#include "audio_io.h"
#include "filter.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 64 //can also be 1024.  Larger means more data, but less callbacks.  
#define NUMCHAN 2 // TODO: get from config
const float DEV_GAIN = -12;
volatile sig_atomic_t sigint_flag = 0;

static void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        // goto err;
        exit(EXIT_FAILURE);
    }
}

void sig_handle(int sig) {
    // see https://stackoverflow.com/questions/17766550/ctrl-c-interrupt-event-handling-in-linux
    printf("\nDetected signal %d\n", sig);
    sigint_flag = 1;
}
//inline to speed up compiler
inline float max(float a, float b) {
    return a > b ? a : b;
}


static int paCallback( // TODO review why these are void*
    const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
    void* userData
) {
    // filters
    ModifiedBiquad *low_shelf_filter = (ModifiedBiquad*)userData;
    float* in = (float*)inputBuffer;
    float* out = (float*)outputBuffer;
    // (void)outputBuffer;
    // for (int i=0; i<framesPerBuffer; i++)
    // {
    //     std::cout << in[i] << " ";
    // }
    int dispSize = 100;
    printf("\r");

    float vol_l = 0;
    float vol_r = 0;

    for (unsigned long i = 0; i < framesPerBuffer*2; i += 2) {
        vol_l = max(vol_l, std::abs(in[i]));
        vol_r = max(vol_r, std::abs(in[i+1]));
        low_shelf_filter->setParameters(DEV_GAIN);
        float filtered = low_shelf_filter->filter(in[i]);
        out[i] = filtered;
        out[i+1] = filtered;
        // out[i] = in[i];
        // out[i+1] = in[i];
    }

    for (int i = 0; i < dispSize; i++) {
        float barProportion = i / (float)dispSize;
        if (barProportion <= vol_l) {
            printf("█");
        }
    }

    fflush(stdout);
    return EXIT_SUCCESS;
}

int main() {
    //filters
    ModifiedBiquad *low_shelf_filter = new ModifiedBiquad(low_shelf);
    ModifiedBiquad *high_shelf_filter = new ModifiedBiquad(high_shelf);
    ModifiedBiquad *peaking_filter = new ModifiedBiquad(peaking);
    PaError err;
    err = Pa_Initialize();
    checkErr(err);
    setUpDevicesOut devices = setUpDevices();

    PaStream* stream;
    err = Pa_OpenStream(
        &stream,
        &devices.inputParams,
        &devices.outputParams,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        paCallback,
        low_shelf_filter
    );
    checkErr(err);

    err = Pa_StartStream(stream);
    checkErr(err);
    // Pa_Sleep(10*1000);
    signal(SIGINT, sig_handle);
    while(1)
        if (sigint_flag) {
            break;
        }
    err = Pa_StopStream(stream);
    checkErr(err);
    err = Pa_CloseStream(stream);
    checkErr(err);

    err = Pa_Terminate();
    checkErr(err);
    printf("Program stopped.\n");
    delete low_shelf_filter;
    return EXIT_SUCCESS;
}