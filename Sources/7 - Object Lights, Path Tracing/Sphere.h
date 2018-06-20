/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "ObjectBase.h"

#include "Math.h"

class Sphere : public ObjectBase
{
public:
    Sphere() : ObjectBase()
    {
        
    }

    Sphere(const Matrix &transformation) : ObjectBase(transformation)
    {
        
    }
    
    ~Sphere() override
    {

    }

    void CreateBVH() override;

    bool Intersection(const Ray &ray, float &t, Vector3 &n, float &beta, float &gamma, const ObjectBase ** hitObject, bool shadowCheck = false) const override;

    void GetIntersectingUV(const Vector3 &intersectionPoint, float beta, float gamma, float &u, float &v) const override;
    Vector3 GetTextureColorAt(const Vector3 &intersectionPoint, float beta, float gamma) const override;

    Vector3 center;
    float radius;

private:
};

#endif