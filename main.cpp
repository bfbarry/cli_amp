#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cmath>
#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512 //can also be 1024.  Larger means more data, but less callbacks.  

static void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

//inline to speed up compiler
inline float max(float a, float b) {
    return a > b ? a : b;
}


static int patestCallback(
    const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
    void* userData
) {
    int numChan = 1; //for mac_mic
    float* in = (float*)inputBuffer; 
    //cast unused vars to void to avoid warnings
    (void)outputBuffer;

    int dispSize = 100;
    printf("\r");
    float vol_l = 0; // only one channel for now
    // float vol_r = 0;
    //i += 2 for double channels
    for (unsigned long i=0; i < framesPerBuffer*numChan; i++) {
        vol_l = max(vol_l, std::abs(in[i]));
        // vol_r = max(vol_r, std::abs(in[i+1])); // i += 2 case
    }
    for (int i = 0; i < dispSize; i++) {
        float barProportion = i/(float)dispSize;
        if (barProportion <= vol_l ) {
            printf("█");
        } else {
            printf(" "); 
        }
    }

    fflush(stdout);
    return EXIT_SUCCESS;
}

int main() {
    PaError err;
    err = Pa_Initialize();
    checkErr(err);

    int numDevices = Pa_GetDeviceCount();
    printf("total devices: %i\n", numDevices);
    if (numDevices < 0) {
        printf("Error getting device count.\n");
        exit(EXIT_FAILURE);
    } else if (numDevices == 0) {
        printf("No devices.\n");
        exit(EXIT_SUCCESS);
    }

    const PaDeviceInfo* deviceInfo; // can't modify the obj but can modify the pointer
    for (int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        printf("Device Num %i\n", i);
        printf("  name %s\n", deviceInfo->name);
        printf("  maxInputChannels %i\n", deviceInfo->maxInputChannels);
        printf("  maxOutputChannels %i\n", deviceInfo->maxOutputChannels);
        printf("  defaultSampleRate %f \n", deviceInfo->defaultSampleRate);
        printf("\n"); 
    }

    int mac_microphone = 0;
    // equivalent of memset(&inputparams, 0, sizeof(inputparams)), structs don't initialize values so do this to be safe
    PaStreamParameters inputParams{};
    PaStreamParameters outputParams{};
    inputParams.channelCount = 1;
    inputParams.device = mac_microphone;
    inputParams.hostApiSpecificStreamInfo = NULL; 
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(mac_microphone)->defaultLowInputLatency; 

    outputParams.channelCount = 0; // just can't be 0
    outputParams.device = mac_microphone;
    outputParams.hostApiSpecificStreamInfo = NULL; 
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(mac_microphone)->defaultLowInputLatency;

    PaStream* stream;
    err = Pa_OpenStream(
        &stream,
        &inputParams,
        &outputParams,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        patestCallback,
        NULL
    );
    checkErr(err);

    err = Pa_StartStream(stream);
    checkErr(err);
    printf("here");
    Pa_Sleep(10*1000);
    err = Pa_StopStream(stream);
    checkErr(err);
    err = Pa_CloseStream(stream);
    checkErr(err);

    err = Pa_Terminate();
    checkErr(err);
    return EXIT_SUCCESS;
}