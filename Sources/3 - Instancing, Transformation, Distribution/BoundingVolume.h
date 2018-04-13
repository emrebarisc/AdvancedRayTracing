/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __BOUNDINGVOLUME_H__
#define __BOUNDINGVOLUME_H__

#include "ObjectBase.h"

#include "Math.h"

class BoundingVolume : public ObjectBase
{
public:
    BoundingVolume() : min(Vector3::ZeroVector), max(Vector3::ZeroVector)
    {

    }

    BoundingVolume(const Matrix &transformation) : ObjectBase(transformation)
    {
        
    }

    BoundingVolume(const Vector3 &minValue, const Vector3 &maxValue) : min(minValue), max(maxValue)
    {

    }

    BoundingVolume(const BoundingVolume& rhs) : ObjectBase(rhs), min(rhs.min), max(rhs.max), left(rhs.left), right(rhs.right)
    {
        
    }

    BoundingVolume& operator=(const BoundingVolume& rhs)
    {
        min = rhs.min;
        max = rhs.max;
        left = rhs.left;
        right = rhs.right;
        return *this;
    }

    ~BoundingVolume()
    {
        delete left;
        delete right;
    }

    Vector3 GetCentroid() override;

    bool Intersection(const Ray &ray, float &t, Vector3& n, float time = 0, bool shadowCheck = false) const override;

    Vector3 min;
    Vector3 max;

    ObjectBase *left, *right;
};

#endif