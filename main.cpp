#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cmath>
#include <portaudio.h>
#include <string>
#include <iostream>
#include <signal.h>


#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 128 //can also be 1024.  Larger means more data, but less callbacks.  
//TODO: select options for DEVICE_IN and DEVICE_OUT, get headphones
#define DEVICE_IN "Steinberg UR22C"
#define DEVICE_OUT "MacBook Pro Speakers"
#define NUMCHAN 2
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


static int patestCallback(
    const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
    void* userData
) {

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

    for (unsigned long i = 0; i < framesPerBuffer*2; i +=2) {
        vol_l = max(vol_l, std::abs(in[i]));
        vol_r = max(vol_r, std::abs(in[i+1]));
        out[i] = in[i];
        out[i+1] = in[i+1];
    }

    for (int i = 0; i < dispSize; i++) {
        float barProportion = i / (float)dispSize;
        if (barProportion <= vol_l && barProportion <= vol_r) {
            printf("█");
        } else if (barProportion <= vol_l) {
            printf("▀");
        } else if (barProportion <= vol_r) {
            printf("▄");
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
    int device_in;
    int device_out;
    // std::string device_name = "Steinberg UR22C";    
    for (int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        //TODO make func to print this info
        if (strcmp(deviceInfo->name, DEVICE_IN) == 0) {
            device_in = i;
            std::string device_name = deviceInfo->name;
            printf("Input device # %i\n", i);
            printf("  name %s\n", deviceInfo->name);
            printf("  maxInputChannels %i\n", deviceInfo->maxInputChannels);
            printf("  maxOutputChannels %i\n", deviceInfo->maxOutputChannels);
            printf("  defaultSampleRate %f \n", deviceInfo->defaultSampleRate);
            printf("\n"); 
        }
        else if (strcmp(deviceInfo->name, DEVICE_OUT) == 0) {
            device_out = i;
            std::string device_name = deviceInfo->name;
            printf("Input device # %i\n", i);
            printf("  name %s\n", deviceInfo->name);
            printf("  maxInputChannels %i\n", deviceInfo->maxInputChannels);
            printf("  maxOutputChannels %i\n", deviceInfo->maxOutputChannels);
            printf("  defaultSampleRate %f \n", deviceInfo->defaultSampleRate);
            printf("\n"); 
        }
    }

    // equivalent of memset(&inputparams, 0, sizeof(inputparams)), structs don't initialize values so do this to be safe
    PaStreamParameters inputParams{};
    PaStreamParameters outputParams{};
    inputParams.channelCount = 2;
    inputParams.device = device_in;
    inputParams.hostApiSpecificStreamInfo = NULL; 
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(device_in)->defaultLowInputLatency; 

    outputParams.channelCount = 2; // just can't be 0
    outputParams.device = device_out;
    outputParams.hostApiSpecificStreamInfo = NULL; 
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(device_out)->defaultLowOutputLatency;

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
    return EXIT_SUCCESS;
}