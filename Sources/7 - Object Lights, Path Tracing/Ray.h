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

    static inline Vector3 GetRandomDirection(const Vector3 &normal)
    {
        // Create Coordinate System u and v
        Vector3 randomRayDirection = normal;
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

    static inline Vector3 GetRandomHemiSphericalDirection(const Vector3 &normal)
    {
        // Create coordinate system
        Vector3 u, v;
        if (std::fabs(normal.x) > std::fabs(normal.y))
        {
            u = Vector3(normal.z, 0, -normal.x) / sqrtf(normal.x * normal.x + normal.z * normal.z); 
        }
        else
        {
            u = Vector3(0, -normal.z, normal.y) / sqrtf(normal.y * normal.y + normal.z * normal.z); 
        }
        v = Vector3::Cross(normal, u); 

        float randomValue1 = RandomGenerator::GetRandomFloat(); 
        float randomValue2 = RandomGenerator::GetRandomFloat();

        float sinTheta = sqrtf(1 - randomValue1 * randomValue1); 
        float phi = 2 * M_PI * randomValue2; 
        float x = sinTheta * cosf(phi); 
        float z = sinTheta * sinf(phi); 
        Vector3 sample = Vector3(x, randomValue1, z); 
        
        return Vector3( sample.x * u.x + sample.y * normal.x + sample.z * v.x, 
                        sample.x * u.y + sample.y * normal.y + sample.z * v.y, 
                        sample.x * u.z + sample.y * normal.z + sample.z * v.z);
    }

    // Can they be constant references?
    Vector3 e;
    Vector3 dir;

    ObjectBase *insideOf;
private:

};

#endif