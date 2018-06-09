/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "AreaLight.h"
#include <random>

std::random_device rd;
std::mt19937 rg(rd());
std::uniform_real_distribution<float> ud(0.0, 1.0);

Vector3 AreaLight::GetPosition() const
{
    float randomU = ud(rg);
    float randomV = ud(rg);

    return position + edgeVectorU * randomU + edgeVectorV * randomV;
}

Vector3 AreaLight::GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const
{
    Vector3 l = lightPosition - positionAt;
    float distance = l.Length();

    if(distance < EPSILON) return intensity;

    return (intensity * Vector3::Dot(l.GetNormalized(), lightNormal)) / (distance * distance);
}
