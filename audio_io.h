#include <portaudio.h>
#include <string>
#include <map>
#pragma once



typedef struct setUpDevicesOut {
    PaStreamParameters inputParams;
    PaStreamParameters outputParams;
} setUpDevicesOut;
setUpDevicesOut setUpDevices();

typedef struct DeviceInfoDict {
    int id;
    int maxInputChannels;
    int maxOutputChannels;
    double defaultSampleRate;
} DeviceInfoDict;
typedef std::map<std::string, DeviceInfoDict> DeviceTable;