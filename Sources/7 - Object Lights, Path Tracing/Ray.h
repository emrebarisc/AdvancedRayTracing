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

    // Can they be constant references?
    Vector3 e;
    Vector3 dir;

    ObjectBase *insideOf;
private:

};

#endif