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
#include "Matrix.h"

struct Face : public ObjectBase
{
    Face() : v0(0), v1(0), v2(0), normal(Vector3::ZeroVector)
    {
        
    }

    ~Face() override
    {
        
    }

    bool Intersection(const Ray& ray, float &t, Vector3& n, bool shadowCheck = false) const override;

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

    ~Mesh() override
    {
        
    }

    void CreateBVH() override;

    bool Intersection(const Ray& ray, float &t, Vector3& n, bool shadowCheck = false) const override;

    Vector3 GetCentroid() override;

    void GetBoundingVolumePositions(Vector3 &min, Vector3 &max) override;
    
    std::vector<Face *> faces;
private:

};

class MeshInstance : public ObjectBase
{
public:
    MeshInstance()
    {

    }

    bool Intersection(const Ray& ray, float &t, Vector3& n, bool shadowCheck = false) const override;

    Mesh* baseMesh;
    Matrix transformationMatrix;
private:

};

#endif