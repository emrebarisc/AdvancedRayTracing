/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "AreaLight.h"

#include "RandomGenerator.h"

Vector3 AreaLight::GetPosition() const
{
    float randomU = RandomGenerator::GetRandomFloat();
    float randomV = RandomGenerator::GetRandomFloat();

    return position + edgeVectorU * randomU + edgeVectorV * randomV;
}

Vector3 AreaLight::GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const
{
    Vector3 l = lightPosition - positionAt;
    float distance = l.Length();

    if(distance < EPSILON) return intensity;

    return (intensity * Vector3::Dot(l.GetNormalized(), lightNormal)) / (distance * distance);
}
