/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __MESH_H__
#define __MESH_H__

#include <vector>

#include "ObjectBase.h"
#include "Math.h"

struct Face : public ObjectBase
{
    Face() : v0(0), v1(0), v2(0), normal(Vector3::ZeroVector())
    {
        
    }

    ~Face()
    {
        
    }

    bool Intersection(const Vector3& e, const Vector3& d, float &t, Vector3& n, bool shadowCheck = false) const override;

    Vector3 GetCentroid() override;

    void GetBoundingVolumePositions(Vector3 &min, Vector3 &max) override;

    unsigned int v0;
    unsigned int v1;
    unsigned int v2;

    Vector3 normal;
};

class Mesh : public ObjectBase
{
public:
    Mesh()
    {

    }

    ~Mesh()
    {
        
    }

    void CreateBVH() override;

    bool Intersection(const Vector3& e, const Vector3& d, float &t, Vector3& n, bool shadowCheck = false) const override;

    Vector3 GetCentroid() override;

    void GetBoundingVolumePositions(Vector3 &min, Vector3 &max) override;
    
    std::vector<Face *> faces;
private:

};

#endif