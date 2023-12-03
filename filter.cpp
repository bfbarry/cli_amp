#include "filter.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

ModifiedBiquad::ModifiedBiquad(ModifiedBiquadFilterType filterType): type(filterType) {
    std::fill(std::begin(this->x_cache), std::end(this->x_cache), 0.0f);
};

float ModifiedBiquad::difference_kernel() {
    float* x = this->x_cache;
    float* y = this->y_cache;
    float &a0 = transfer_coeffs.a0;
    float &a1 = transfer_coeffs.a1;
    float &a2 = transfer_coeffs.a2;
    float &b1 = transfer_coeffs.b1;
    float &b2 = transfer_coeffs.b2;
    float &c0 = transfer_coeffs.c0;
    float &d0 = transfer_coeffs.d0;
    float yn = (c0*a0+d0)*x[2] + (c0*a1+d0*b1)*x[1] + (c0*a2+d0*b2)*x[0] - b1*y[1] -b2*y[0];
    float yn0_tmp = y[1];
    this->y_cache[1] = yn;
    this->y_cache[0] = yn0_tmp;
    return yn;
};

void ModifiedBiquad::set_coeffs_first_order_low_shelf() {
    Transfer_coefficients & tc = this->transfer_coeffs;

    float theta_c = (2.*PI*this->fc)/FS;
    float mu = pow(10., (this->gain/20.));
    float beta = 4./(1.+mu);
    float delta = beta*tan(theta_c/2.);
    float gamma = (1-delta)/(1+delta);
    
    tc.a0 = (1-gamma)/2;
    tc.a1 = tc.a0 ;
    tc.a2 = 0.;
    tc.b1 = -gamma;
    tc.b2 = 0.;
    tc.c0 = mu - 1.;
    tc.d0 = 1.;
};

void ModifiedBiquad::set_coeffs_first_order_high_shelf() {
    Transfer_coefficients & tc = this->transfer_coeffs;

    float theta_c = (2.*PI*this->fc)/FS;
    float mu = pow(10., (this->gain/20.));
    float beta = (1.+mu)/4.;
    float delta = beta*tan(theta_c/2.);
    float gamma = (1-delta)/(1+delta);
    tc.a0 = (1+gamma)/2;
    tc.a1 = (-gamma-1)/2;
    tc.a2 = 0.;
    tc.b1 = -gamma;
    tc.b2 = 0.;
    tc.c0 = mu - 1.;
    tc.d0 = 1.;
};

void ModifiedBiquad::set_coeffs_second_order_constant_peaking() {
    Transfer_coefficients & tc = this->transfer_coeffs;
    enum BoostCut { boost, cut };
    BoostCut gain_rule;
    if (this->gain < 0) {
        gain_rule = cut;
    }
    else {
        gain_rule = boost;
    }
    float Q = .7;
    float K = tan((PI*fc)/FS);
    float V0 = pow(10., (gain/20));
    float d0 = 1 + (1/Q)*K + pow(K, 2);
    float e0 = 1 + (1/(V0*Q))*K + pow(K, 2);
    float alpha = 1 + (V0/Q)*K + pow(K, 2);
    float beta = 2*(pow(K, 2)-1);
    float gamma = 1 - (V0/Q)*K + pow(K, 2);
    float delta = 1 - (1/Q)*K + pow(K, 2);
    float eta = 1 - (1/(V0*Q))*K + pow(K, 2);

    tc.c0 = 1.l;
    switch (gain_rule) {
        case boost:
            tc.a0 = alpha/d0;
            tc.a1 = beta/d0;
            tc.a2 = gamma/d0;
            tc.b1 = beta/d0;
            tc.b2 = delta/d0;
            break;
        case cut:
            tc.a0 = d0/e0;
            tc.a1 = beta/e0;
            tc.a2 = delta/e0;
            tc.b1 = beta/e0;
            tc.b2 = eta/e0;
            break;
    };
    tc.d0 = 0.;
};

float ModifiedBiquad::filter(float xn) {
    float xn1_tmp = this->x_cache[2];
    float xn0_tmp = this->x_cache[1];
    this->x_cache[2] = xn;
    this->x_cache[1] = xn1_tmp;
    this->x_cache[0] = xn0_tmp;
    float sample_out = difference_kernel();
    return sample_out;
};

void ModifiedBiquad::setParameters(float gain) {
    if (gain < -12. || gain > 12) {
        throw std::invalid_argument("gain must be in [-12, 12]");
    }
    this->gain = gain;
    switch(this->type) {
        case low_shelf:
            this->fc = 100.;
            this->set_coeffs_first_order_low_shelf();
            break;
        case high_shelf:
            this->fc = 750.;
            this->set_coeffs_first_order_high_shelf();
            break;
        case peaking:
            this->fc = 3000.;
            this->set_coeffs_second_order_constant_peaking();
            break;
    };
};

Distortion::Distortion(DistortionFilterType filterType): type(filterType) {

};

void Distortion::setParameters(float gain) {
    if (gain < -12. || gain > 12) {
        throw std::invalid_argument("gain must be in [-12, 12]");
    }
    this->gain = gain;
};

float Distortion::filter(float xn) {
    float sample_out;
    switch(this->type) {
        case exponential:
            sample_out = explonential_filter(xn);
            break;
    };
    return sample_out;
};

float Distortion::explonential_filter(float xn) {
    float yn;
    float C = this->gain;
    if (C == 0.) {
        return xn;
    }
    if (xn > 0) {
        yn = 1- exp(C*-xn);
    } else {
        yn = -1 + exp(C*-xn);
    }
    return yn;
};