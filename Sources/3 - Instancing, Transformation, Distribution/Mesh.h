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
    Face() : ObjectBase(), v0(0), v1(0), v2(0), normal(Vector3::ZeroVector)
    {
        
    }

    Face(const Matrix &transformation) : ObjectBase(transformation)
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
    Mesh() : ObjectBase()
    {

    }

    Mesh(const Matrix &transformation) : ObjectBase(transformation)
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
    MeshInstance() : ObjectBase()
    {

    }

    MeshInstance(const Mesh *mesh, const Matrix &transformation) : ObjectBase(transformation), baseMesh(mesh)
    {

    }

    bool Intersection(const Ray& ray, float &t, Vector3& n, bool shadowCheck = false) const override;

    const Mesh* baseMesh;
private:

};

#endif