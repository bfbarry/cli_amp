#include "filter.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

ModifiedBiquad::ModifiedBiquad(FilterType filterType): type(filterType) {
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
    return (c0*a0+d0)*x[2] + (c0*a1+d0*b1)*x[1] + (c0*a2+d0*b2)*x[0] - b1*y[1] -b2*y[0];
};

void ModifiedBiquad::set_coeffs_first_order_low_shelf() {
    float theta_c = (2.*PI*this->fc)/FS;
    float mu = pow(10., (this->gain/20.));
    float beta = 4./(1.+mu);
    float delta = beta*tan(theta_c/2.);
    float gamma = (1-delta)/(1+delta);
    
    Transfer_coefficients & tc = this->transfer_coeffs;
    tc.a0 = (1-gamma)/2;
    tc.a1 = tc.a0 ;
    tc.a2 = 0.;
    tc.b1 = -gamma;
    tc.b2 = 0.;
    tc.c0 = mu - 1.;
    tc.d0 = 1.;
};

void ModifiedBiquad::set_coeffs_first_order_high_shelf() {};

void ModifiedBiquad::set_coeffs_second_order_constant_peaking() {};

float ModifiedBiquad::filter(float xn) {
    float &xn1_tmp = this->x_cache[2];
    float &xn2_tmp = this->x_cache[1];
    this->x_cache[2] = xn;
    x_cache[2] = xn;
    float sample_out = difference_kernel();
    return sample_out;
};

void ModifiedBiquad::setParameters(float fc, float gain) {
    if (gain < -12. || gain > 12) {
        throw std::invalid_argument("gain must be in [-12, 12]");
    }
    this->fc = fc;
    this->gain = gain;
    switch(this->type) {
        case low_shelf:
            set_coeffs_first_order_low_shelf();
            break;
        case high_shelf:
            set_coeffs_first_order_high_shelf();
            break;
        case peak:
            set_coeffs_second_order_constant_peaking();
            break;
    };
};

