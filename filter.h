#include <string>
#define FS 44100

typedef struct Transfer_coefficients {
    int a0;
    int a1;
    int a2;
    int b1;
    int b2;
    int c0;
    int d0;
} Transfer_coefficients;

enum FilterType { low_shelf, high_shelf, peak};

class ModifiedBiquad {
    private:
        FilterType type;
        Transfer_coefficients transfer_coeffs;
        float fc;
        float gain;
        float x_cache[3] = {0, 0, 0};
        float y_cache[3] = {0, 0, 0};
        float difference_kernel(float* x, float* y) {
            return (c0*a0+d0)*x[i] + (c0*a1+d0*b1)*x[i-1] + (c0*a2+d0*b2)*x[i-2] - b1*y[i-1] -b2*y[i-2];
        };
    public:
        float filter(float xn) {
            float xn1_tmp = this->x_cache[2];
            float xn2_tmp = this->x_cache[1];
            this->x_cache[2] = xn;
            x_cache[2] = xn;
        };

        void setParameters(float fc, float gain);
        Transfer_coefficients first_order_low_shelf() {

        };


        
};