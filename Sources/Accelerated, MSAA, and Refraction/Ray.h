/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __RAY_H__
#define __RAY_H__

#include "Math.h"

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


    Vector3 e;
    Vector3 dir;

    ObjectBase *insideOf;
private:

};

#endif
