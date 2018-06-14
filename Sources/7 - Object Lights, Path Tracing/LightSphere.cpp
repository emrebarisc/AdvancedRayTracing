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

    Vector3 transformedPosition = inverseTransformationMatrix * Vector4(lightPosition, 1.f);

    return transformedPosition;
}