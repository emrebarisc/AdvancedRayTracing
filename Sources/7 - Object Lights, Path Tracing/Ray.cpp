#include "Ray.h"

#include "RandomGenerator.h"

Vector3 Ray::GetRandomDirection(const Vector3 &normal)
{
    // step 1: draw a random sample in the half-disk
    float theta = RandomGenerator::GetRandomFloat() * TWO_PI;
    float cosTheta = cos(theta); 
    float sinTheta = sin(theta); 
    // step 2: rotate the sample direction
    float sx = cosTheta  * normal.y + sinTheta  * -normal.x; 
    float sy = cosTheta  * normal.x + sinTheta  * normal.y; 

    Vector3 randomDirection = Vector3(sx, sy, normal.z).GetNormalized();

    return randomDirection;
}