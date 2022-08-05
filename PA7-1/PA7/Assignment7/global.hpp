#pragma once
#include <iostream>
#include <cmath>
#include <random>

#undef M_PI
#define M_PI 3.141592653589793f

extern const float  EPSILON;
const float kInfinity = std::numeric_limits<float>::max();
inline float clamp(const float &lo, const float &hi, const float &v)
{ return std::max(lo, std::min(hi, v)); }

inline  bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1)
{
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = - 0.5 * b / a;
    else {
        float q = (b > 0) ?
                  -0.5 * (b + sqrt(discr)) :
                  -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) std::swap(x0, x1);
    return true;
}

inline float get_random_float()
{
    static std::random_device dev;
    static std::mt19937 rng(dev());
    static std::uniform_real_distribution<float> dist(0.f, 1.f); // distribution in range [1, 6]

    return dist(rng);
}

inline void UpdateProgress(float progress)
{
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
};

inline Vector3f random_in_unit_sphere()
{   
    Vector3f p = 2.0 * Vector3f(get_random_float(), get_random_float(), get_random_float()) - Vector3f(1, 1, 1);
    while(p.norm() >= 1.0)
    {
        p = 2.0 * Vector3f(get_random_float(), get_random_float(), get_random_float()) - Vector3f(1, 1, 1);
    }
    return p;
}

inline Vector3f random_unit_vector()
{
    return random_in_unit_sphere().normalized();
}

inline float reflectance(float cos_theta, float ref_idx)
{
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow(1 - cos_theta, 5);
}

inline Vector3f my_refract(Vector3f I, Vector3f N, float ior)
{
    float cos_theta = std::min(dotProduct(N, -I), 1.0f);
    Vector3f r_out_perp = ior * (I + cos_theta * N);
    Vector3f r_out_parallel = -std::sqrt(std::abs(1.0f - dotProduct(r_out_perp, r_out_perp))) * N;
    return r_out_parallel + r_out_perp; 
}