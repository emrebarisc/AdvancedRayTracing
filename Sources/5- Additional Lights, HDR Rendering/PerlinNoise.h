/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __PERLINNOISE_H__
#define __PERLINNOISE_H__

#include "Texture.h"

#include <array>

#include "Math.h"

class PerlinNoise : public Texture
{
public:
    PerlinNoise();
    PerlinNoise(unsigned int seed);
    ~PerlinNoise()
    {
        
    }
    
    Vector3 GetInterpolatedUV(float u, float v, float w = 0.f ) const override;
    float GetPerlinNoiseAt(float x, float y, float z = 0.f) const;

    float scalingFactor = 1.f;
private:
    inline float WeightFunction(float value) const
    {
        float absValue = mathAbs(value);
        float absValueCube = absValue * absValue * absValue;
        return (-6.f * absValueCube * absValue * absValue) + (15.f * absValueCube * absValue) - (10.f * absValueCube) + 1.f;
    }

    std::array<unsigned int, 16> p{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    Vector3 edgeVectors[16] = { Vector3(0.707106769f, 0.707106769f, 0.f), Vector3(-0.707106769f, 0.707106769f, 0.f), Vector3(0.707106769f, -0.707106769f, 0.f), Vector3(-0.707106769f, -0.707106769f, 0.f), 
                                Vector3(0.707106769f, 0.f, 0.707106769f), Vector3(-0.707106769f, 0.f, 0.707106769f), Vector3(0.707106769f, 0.f, -0.707106769f), Vector3(-0.707106769f, 0.f, -0.707106769f),
                                Vector3(0.f, 0.707106769f, 0.707106769f), Vector3(0.f, -0.707106769f, 0.707106769f), Vector3(0.f, 0.707106769f, -0.707106769f), Vector3(0.f, -0.707106769f, -0.707106769f),
                                Vector3(0.707106769f, 0.707106769f, 0.f), Vector3(-0.707106769f, 0.707106769f, 0.f), Vector3(0.f, -0.707106769f, 0.707106769f), Vector3(0.f, -0.707106769f, -0.707106769f) };
};

#endif