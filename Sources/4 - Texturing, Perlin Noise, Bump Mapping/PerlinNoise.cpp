/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "PerlinNoise.h"

#include <algorithm>
#include <chrono>
#include <random>

#define IndexFunction(value) (value % 16)

PerlinNoise::PerlinNoise()
{
    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle (p.begin(), p.end(), std::default_random_engine(seed));
}

PerlinNoise::PerlinNoise(unsigned int seed)
{
    std::shuffle (p.begin(), p.end(), std::default_random_engine(seed));
}

Vector3 PerlinNoise::GetInterpolatedUV(float u, float v, float z/*  = 0.f */) const 
{
    float perlinValue = GetPerlinNoiseAt(u, v, z);

    if(appearance == APPEARANCE::VEIN)
    {
        perlinValue = mathAbs(perlinValue);
    }
    else
    {
        perlinValue = (perlinValue + 1) * 0.5f;
    }

    return Vector3(255.f * perlinValue);
}

float PerlinNoise::GetPerlinNoiseAt(float x, float y, float z/*  = 0.f */) const
{
    x *= scalingFactor;
    y *= scalingFactor;
    z *= scalingFactor;

    int floorX = floor(x);
    int floorY = floor(y);
    int floorZ = floor(z);

    Vector3 point = Vector3(x, y, z);

    const Vector3 e1 = edgeVectors[ p[(floorX     + p[(floorY     + p[floorZ % 16]) % 16]) % 16] ];
    const Vector3 e2 = edgeVectors[ p[(floorX + 1 + p[(floorY     + p[floorZ % 16]) % 16]) % 16] ];
    const Vector3 e3 = edgeVectors[ p[(floorX     + p[(floorY + 1 + p[floorZ % 16]) % 16]) % 16] ];
    const Vector3 e4 = edgeVectors[ p[(floorX + 1 + p[(floorY + 1 + p[floorZ % 16]) % 16]) % 16] ];
    const Vector3 e5 = edgeVectors[ p[(floorX     + p[(floorY     + p[(floorZ + 1) % 16]) % 16]) % 16] ];
    const Vector3 e6 = edgeVectors[ p[(floorX + 1 + p[(floorY     + p[(floorZ + 1) % 16]) % 16]) % 16] ];
    const Vector3 e7 = edgeVectors[ p[(floorX     + p[(floorY + 1 + p[(floorZ + 1) % 16]) % 16]) % 16] ];
    const Vector3 e8 = edgeVectors[ p[(floorX + 1 + p[(floorY + 1 + p[(floorZ + 1) % 16]) % 16]) % 16] ];

    const Vector3 v1 = point - Vector3(floorX, floorY, floorZ);
    const Vector3 v2 = point - Vector3(floorX + 1, floorY, floorZ);
    const Vector3 v3 = point - Vector3(floorX, floorY + 1, floorZ);
    const Vector3 v4 = point - Vector3(floorX + 1, floorY + 1, floorZ);
    const Vector3 v5 = point - Vector3(floorX, floorY, floorZ + 1);
    const Vector3 v6 = point - Vector3(floorX + 1, floorY, floorZ + 1);
    const Vector3 v7 = point - Vector3(floorX, floorY + 1, floorZ + 1);
    const Vector3 v8 = point - Vector3(floorX + 1, floorY + 1, floorZ + 1);
 
    return Vector3::Dot(e1, v1) * WeightFunction(v1.x) * WeightFunction(v1.y) * WeightFunction(v1.z) +
           Vector3::Dot(e2, v2) * WeightFunction(v2.x) * WeightFunction(v2.y) * WeightFunction(v2.z) +
           Vector3::Dot(e3, v3) * WeightFunction(v3.x) * WeightFunction(v3.y) * WeightFunction(v3.z) +
           Vector3::Dot(e4, v4) * WeightFunction(v4.x) * WeightFunction(v4.y) * WeightFunction(v4.z) +

           Vector3::Dot(e5, v5) * WeightFunction(v5.x) * WeightFunction(v5.y) * WeightFunction(v5.z) +
           Vector3::Dot(e6, v6) * WeightFunction(v6.x) * WeightFunction(v6.y) * WeightFunction(v6.z) +
           Vector3::Dot(e7, v7) * WeightFunction(v7.x) * WeightFunction(v7.y) * WeightFunction(v7.z) +
           Vector3::Dot(e8, v8) * WeightFunction(v8.x) * WeightFunction(v8.y) * WeightFunction(v8.z);
}