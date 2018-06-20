/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "LightSphere.h"

#include "RandomGenerator.h"

// Reference: mathworld.wolfram.com/SpherePointPicking.html
Vector3 LightSphere::GetPosition() const
{
    float randomU = RandomGenerator::GetRandomFloat(-1.f, 1.f);
    float randomV = RandomGenerator::GetRandomFloat(-1.f, 1.f);
    float randomW = RandomGenerator::GetRandomFloat(-1.f, 1.f);

    float divider = sqrt(randomU * randomU + randomV * randomV + randomW * randomW);

    randomU /= divider;
    randomV /= divider;
    randomW /= divider;

    Vector3 lightPosition = Vector3(randomU, randomV, randomW) * radius;
    lightPosition += center;

    return Vector3(transformationMatrix * Vector4(lightPosition, 1.f));
}

Vector3 LightSphere::GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const
{
    float distance = (lightPosition - positionAt).Length();
    return intensity / (distance * distance);

    // We actually return radiance 
    //return intensity;
}