#include <string>
#pragma once
#define FS 44100
constexpr float PI = 3.1415927f;

typedef struct Transfer_coefficients {
    float a0;
    float a1;
    float a2;
    float b1;
    float b2;
    float c0;
    float d0;
} Transfer_coefficients;


class DspFilter {
    protected:
        float gain;
    public:
        float filter(float xn);
        void setParameters(float gain);
};

enum ModifiedBiquadFilterType { low_shelf, high_shelf, peaking };

class ModifiedBiquad: DspFilter {
    private:
        ModifiedBiquadFilterType type;
        Transfer_coefficients transfer_coeffs;
        float fc;
        static const size_t x_len = 3; // or constexpr
        static const size_t y_len = 2;
        float x_cache[x_len];
        float y_cache[y_len];
        float difference_kernel();
        // design equations - set transfer_coeffs
        void set_coeffs_first_order_low_shelf();
        void set_coeffs_first_order_high_shelf();
        void set_coeffs_second_order_constant_peaking();
    public:
        ModifiedBiquad(ModifiedBiquadFilterType filterType);
        void setParameters(float gain);
        float filter(float xn);
};

enum DistortionFilterType { exponential };

class Distortion: DspFilter {
    private:
        DistortionFilterType type;
        float explonential_filter(float xn);
    public:
        Distortion(DistortionFilterType filterType);
        void setParameters(float gain);
        float filter(float xn);
};

typedef struct DspCallbackVessel {
    float low_shelf_gain;
    float high_shelf_gain;
    float peaking_gain;
    float distortion_gain;
    ModifiedBiquad* low_shelf_filter;
    ModifiedBiquad* high_shelf_filter;
    ModifiedBiquad* peaking_filter;
    Distortion* distortion_filter;
} DspCallbackVessel;