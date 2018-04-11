/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __OBJECTBASE_H__
#define __OBJECTBASE_H__

#include "BVH.h"
#include "Math.h"
#include "Matrix.h"
#include "Ray.h"

class Material;

/*
    Base object class
*/
class ObjectBase
{
public:
    ObjectBase() : transformationMatrix(Matrix::IdentityMatrix), inverseTransformationMatrix(Matrix::IdentityMatrix)
    {

    }

    ObjectBase(const ObjectBase &rhs) : bvh(rhs.bvh), material(rhs.material), transformationMatrix(rhs.transformationMatrix), inverseTransformationMatrix(rhs.inverseTransformationMatrix)
    {
        
    }

    ObjectBase(const Matrix &transformation) : transformationMatrix(transformation)
    {
        inverseTransformationMatrix = transformationMatrix.Invert();
    }
    
    virtual ~ObjectBase()
    {

    }

    virtual void CreateBVH()
    {

    }

    virtual Vector3 GetCentroid()
    {
        return Vector3::ZeroVector;
    }

    virtual void GetBoundingVolumePositions(Vector3 &min, Vector3 &max)
    {
        min = Vector3::ZeroVector;
        max = Vector3::ZeroVector;
    }

    virtual bool Intersection(const Ray &ray, float &t, Vector3& n,  bool shadowCheck = false) const = 0;

    BVH bvh;

    Material *material;
    
    Matrix transformationMatrix;
    Matrix inverseTransformationMatrix;

protected:

private:
};

#endif