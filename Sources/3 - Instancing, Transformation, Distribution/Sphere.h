#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "ObjectBase.h"

#include "Math.h"

class Sphere : public ObjectBase
{
public:
    Sphere()
    {
        
    }

    Sphere(const Matrix &transformation) : ObjectBase(transformation)
    {
        
    }
    
    ~Sphere() override
    {

    }

    void CreateBVH() override;

    bool Intersection(const Ray &ray, float &t, Vector3 &n, bool shadowCheck = false) const override;

    Vector3 center;
    float radius;

private:
};

#endif