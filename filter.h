#include <string>
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

enum FilterType { low_shelf, high_shelf, peak };

class ModifiedBiquad {
    private:
        FilterType type;
        Transfer_coefficients transfer_coeffs;
        float fc;
        float gain;
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
        ModifiedBiquad(FilterType filterType);
        float filter(float xn);
        void setParameters(float fc, float gain);
};