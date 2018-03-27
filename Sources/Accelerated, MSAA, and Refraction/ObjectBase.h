/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __OBJECTBASE_H__
#define __OBJECTBASE_H__

#include "BVH.h"
#include "Math.h"

class Material;

/*
    Base object class
*/
class ObjectBase
{
public:
    ObjectBase()
    {

    }
    
    ~ObjectBase()
    {

    }

    virtual void CreateBVH()
    {

    }

    virtual Vector3 GetCentroid()
    {

    }

    virtual void GetBoundingVolumePositions(Vector3 &min, Vector3 &max)
    {

    }

    virtual bool Intersection(const Vector3& e, const Vector3& d, float &t, Vector3& n,  bool shadowCheck = false) const = 0;

    BVH bvh;

    Material *material;

private:
};

#endif