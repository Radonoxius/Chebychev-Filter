#include <math.h>
#include <stdint.h>
#include "audio.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define CHEB_ORDER  4
#define NUM_STAGES  (CHEB_ORDER / 2)

typedef struct {
    float b0, b1, b2;
    float a1, a2;
    float s1, s2;
} Biquad;

typedef struct {
    Biquad stages[NUM_STAGES];
    int    num_stages;
} ChebyshevLPF;

void chebyshev_lpf_init(ChebyshevLPF *f, float fs, float fc, float ripple_db)
{
    const int N = CHEB_ORDER;

    float eps = sqrtf(powf(10.0f, ripple_db / 10.0f) - 1.0f);

    float alpha = asinhf(1.0f / eps) / (float)N;

    float K       = 2.0f * fs;
    float Omega_c = K * tanf((float)M_PI * fc / fs);

    f->num_stages = N / 2;

    for (int i = 0; i < f->num_stages; i++) {
        int   k     = i + 1;
        float theta = (float)M_PI * (2*k - 1) / (2*N);

        float sigma = -sinhf(alpha) * sinf(theta);
        float omega =  coshf(alpha) * cosf(theta);

        float p  = -sigma;
        float q  =  fabsf(omega);
        float r2 =  p*p + q*q;

        float Oc2r2 = Omega_c * Omega_c * r2;

        float D0 =  K*K  +  2.0f*Omega_c*p*K  +  Oc2r2;
        float D1 = -2.0f*K*K                   +  2.0f*Oc2r2;
        float D2 =  K*K  -  2.0f*Omega_c*p*K  +  Oc2r2;

        float G = Oc2r2;

        f->stages[i].b0 = G / D0;
        f->stages[i].b1 = 2.0f * G / D0;   // (z+1)² ? 1, 2, 1 pattern
        f->stages[i].b2 = G / D0;
        f->stages[i].a1 = D1 / D0;
        f->stages[i].a2 = D2 / D0;
        f->stages[i].s1 = 0.0f;
        f->stages[i].s2 = 0.0f;
    }

    if (N % 2 == 0) {
        float corr = 1.0f / sqrtf(1.0f + eps * eps);
        f->stages[0].b0 *= corr;
        f->stages[0].b1 *= corr;
        f->stages[0].b2 *= corr;
    }
}

static inline float process_biquad(float x, Biquad *b)
{
    float y = b->b0 * x  +  b->s1;
    b->s1   = b->b1 * x  -  b->a1 * y  +  b->s2;
    b->s2   = b->b2 * x  -  b->a2 * y;
    return y;
}

float chebyshev_lpf_process(float x, ChebyshevLPF *f)
{
    for (int i = 0; i < f->num_stages; i++)
        x = process_biquad(x, &f->stages[i]);
    return x;
}

void chebyshev_lpf_reset(ChebyshevLPF *f)
{
    for (int i = 0; i < f->num_stages; i++)
        f->stages[i].s1 = f->stages[i].s2 = 0.0f;
}

static ChebyshevLPF lpf;

void ProcessData(I2S_Data_TypeDef *I2S_Data)
{
    if (IS_LEFT_CH_SELECT(I2S_Data->mask))
    {
        int16_t input = I2S_Data->input_r;
        float   n     = (float)input / 32768.0f;

        float y = chebyshev_lpf_process(n, &lpf);

        y *= 32768.0f;
        if (y >  32767.0f) y =  32767.0f;
        if (y < -32768.0f) y = -32768.0f;

        I2S_Data->output_l = (int16_t)y;
    }
}

int main(void)
{
    chebyshev_lpf_init(&lpf, 8000.0f, 1000.0f, 1.0f);

    AudioInit(FS_8000_HZ, AUDIO_INPUT_LINE, IO_METHOD_INTR);
    while (1) __wfi();
}
