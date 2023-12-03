#include <portaudio.h>
#include <string>
#include <map>
#include <nlohmann/json.hpp>

#pragma once

typedef struct setUpDevicesOut {
    PaStreamParameters inputParams;
    PaStreamParameters outputParams;
} setUpDevicesOut;

nlohmann::json readConfig(); 

setUpDevicesOut setUpDevices(nlohmann::json config);

typedef struct DeviceInfoDict {
    int id;
    int maxInputChannels;
    int maxOutputChannels;
    double defaultSampleRate;
} DeviceInfoDict;

typedef std::map<std::string, DeviceInfoDict> DeviceTable;