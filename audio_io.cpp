#include <portaudio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include "audio_io.h"


void printDeviceInfo(DeviceInfoDict deviceInfoDict) {
    printf("  maxInputChannels %i\n", deviceInfoDict.maxInputChannels);
    printf("  maxOutputChannels %i\n", deviceInfoDict.maxOutputChannels);
    printf("  defaultSampleRate %f \n", deviceInfoDict.defaultSampleRate);
    printf("\n");
}

void printAllDeviceInfo(DeviceTable deviceTable) {
    for (const auto &kv: deviceTable) {
        const DeviceInfoDict& dict = kv.second;
        printf("  name %s\n", kv.first.c_str());
        printDeviceInfo(dict);
    }
}


DeviceTable getDeviceInfo(int numDevices) {
    DeviceTable deviceTable;
    const PaDeviceInfo* deviceInfo; // can't modify the obj but can modify the pointer
    for (int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        DeviceInfoDict deviceInfoDict = {
                                    i,
                                    deviceInfo->maxInputChannels,
                                    deviceInfo->maxOutputChannels,
                                    deviceInfo->defaultSampleRate,
                                    };

        std::string name = std::string(deviceInfo->name);
        deviceTable.insert({name, deviceInfoDict});
    }
    return deviceTable;
}

nlohmann::json readConfig() {
    nlohmann::json config;

    std::ifstream input_file("config.json");
    if (input_file.is_open()) {
        input_file >> config;
    }
    return config;
};

setUpDevicesOut setUpDevices(nlohmann::json config) {
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        printf("Error getting device count.\n");
        exit(EXIT_FAILURE);
    } else if (numDevices == 0) {
        printf("No devices.\n");
        exit(EXIT_SUCCESS);
    }

    std::string input_device_name = config["io"]["input_device"];
    std::string output_device_name = config["io"]["output_device"];
 
    bool configChanged = false;
    DeviceTable deviceTable = getDeviceInfo(numDevices);
    if (input_device_name.empty() && output_device_name.empty()) {
        printAllDeviceInfo(deviceTable);
        printf("Enter input device name");
        std::getline(std::cin, input_device_name);
        printf("Enter output device name");
        std::getline(std::cin, output_device_name);
        configChanged = true;
    } else if (input_device_name.empty()) {
        printAllDeviceInfo(deviceTable);
        printf("Enter input device name");
        std::getline(std::cin, input_device_name);
        configChanged = true;
    } else if (output_device_name.empty()) {
        printAllDeviceInfo(deviceTable);
        printf("Enter output device name");
        std::getline(std::cin, output_device_name);
        configChanged = true;
    }

    if (configChanged) {
        config["io"]["input_device"] = input_device_name;
        config["io"]["output_device"] = output_device_name;
        std::ofstream output_file("config.json");
        if (output_file.is_open()) {
            output_file << config.dump(4);
        }
    }
    int input_device_id;
    int output_device_id;
    input_device_id = deviceTable[input_device_name].id;
    printf("Input device # %s\n", input_device_name.c_str());
    printDeviceInfo(deviceTable[input_device_name]);
    output_device_id = deviceTable[output_device_name].id;
    printf("Output device # %s\n", output_device_name.c_str());
    printDeviceInfo(deviceTable[output_device_name]);

    // equivalent of memset(&inputparams, 0, sizeof(inputparams)), structs don't initialize values so do this to be safe
    PaStreamParameters inputParams{};
    PaStreamParameters outputParams{};
    inputParams.channelCount = 2;
    inputParams.device = input_device_id;
    inputParams.hostApiSpecificStreamInfo = NULL; 
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(input_device_id)->defaultLowInputLatency; 

    outputParams.channelCount = 2; // just can't be 0
    outputParams.device = output_device_id;
    outputParams.hostApiSpecificStreamInfo = NULL; 
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(output_device_id)->defaultLowOutputLatency;

    setUpDevicesOut r = {inputParams, outputParams};
    return r;
}