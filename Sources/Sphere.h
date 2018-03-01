#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "ObjectBase.h"

#include "Math.h"

class Sphere : public ObjectBase
{
public:
    Sphere();
    ~Sphere();

    bool Intersection(const Vector3& e, const Vector3& d, float &t) override;

    Vector3 center;
    float radius;

private:
};

#endif