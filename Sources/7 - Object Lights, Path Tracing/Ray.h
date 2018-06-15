/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __RAY_H__
#define __RAY_H__

#include "Math.h"
#include "RandomGenerator.h"

class ObjectBase;

class Ray
{
public:
    Ray() : e(Vector3::ZeroVector), dir(Vector3::ZeroVector), insideOf(nullptr)
    {
        
    }

    Ray(const Vector3 &eye, const Vector3 & d, ObjectBase *inside = nullptr) : 
        e(eye), dir(d), insideOf(inside)
    {

    }
    
    ~Ray()
    {
        
    }

    // Returns a direction relatively closer to the reference
    static inline Vector3 GetRandomDirection(const Vector3 &reference)
    {
        // Create Coordinate System u and v
        Vector3 randomRayDirection = reference;
        Vector3 rPrime = randomRayDirection.GetOrthonormalBasis();
        rPrime.Normalize();
        Vector3 u = Vector3::Cross(rPrime, randomRayDirection);
        Vector3 v = Vector3::Cross(u, randomRayDirection);

        float randomU = RandomGenerator::GetRandomFloat();
        float randomV = RandomGenerator::GetRandomFloat();
        randomRayDirection += u * randomU + v * randomV;
        randomRayDirection.Normalize();

        return randomRayDirection;
    }

    // Returns a direction with an angle between 0 and 90 with respect to reference ray
    static inline Vector3 GetRandomHemiSphericalDirection(const Vector3 &referenceRay)
    {
        // Create coordinate system
        Vector3 u, v;
        if (std::fabs(referenceRay.x) > std::fabs(referenceRay.y))
        {
            u = Vector3(referenceRay.z, 0, -referenceRay.x) / sqrtf(referenceRay.x * referenceRay.x + referenceRay.z * referenceRay.z); 
        }
        else
        {
            u = Vector3(0, -referenceRay.z, referenceRay.y) / sqrtf(referenceRay.y * referenceRay.y + referenceRay.z * referenceRay.z); 
        }
        v = Vector3::Cross(referenceRay, u); 

        float randomValue1 = RandomGenerator::GetRandomFloat(); 
        float randomValue2 = RandomGenerator::GetRandomFloat();
    
        float sinTheta = sqrtf(1 - randomValue1 * randomValue1); 
        float phi = 2 * M_PI * randomValue2; 
        float x = sinTheta * cosf(phi);
        float z = sinTheta * sinf(phi);
        Vector3 sample = Vector3(x, randomValue1, z); 
        
        return Vector3( sample.x * u.x + sample.y * referenceRay.x + sample.z * v.x, 
                        sample.x * u.y + sample.y * referenceRay.y + sample.z * v.y, 
                        sample.x * u.z + sample.y * referenceRay.z + sample.z * v.z);
    }

    // Can they be constant references?
    Vector3 e;
    Vector3 dir;

    ObjectBase *insideOf;
private:

};

#endif