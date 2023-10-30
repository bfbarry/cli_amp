#include <stdio.h>
#include <stdlib.h>
#include <portaudio.h>

static void checkErr(PaError err) {
    if (err != paNoError) {
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
        exit(EXIT_FAILURE);
    }
}

int main() {
    PaError err;
    err = Pa_Initialize();
    checkErr(err);
    err = Pa_Terminate();
    checkErr(err);
    return EXIT_SUCCESS;
}

// To start jack now and restart at login:
//   brew services start jack
// Or, if you don't want/need a background service you can just run:
//   /opt/homebrew/opt/jack/bin/jackd -X coremidi -d coreaudio

//   To start pulseaudio now and restart at login:
//   brew services start pulseaudio
// Or, if you don't want/need a background service you can just run:
//   /opt/homebrew/opt/pulseaudio/bin/pulseaudio --exit-idle-time\=-1 --verbose