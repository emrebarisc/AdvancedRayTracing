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

enum SHADING_MODE : uint8_t
{
    FLAT = 0,
    SMOOTH
};

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

    bool Intersection(const Ray& ray, float &t, Vector3& n, float &beta, float &gamma, const ObjectBase ** hitObject, bool shadowCheck = false) const override;

    void GetIntersectingUV(const Vector3 &intersectionPoint, float beta, float gamma, float &u, float &v) const override;
    Vector3 GetTextureColorAt(const Vector3 &intersectionPoint, float beta, float gamma) const override;

    Vector3 GetCentroid() override;

    void GetBoundingVolumePositions(Vector3 &min, Vector3 &max) override;

    unsigned int v0;
    unsigned int v1;
    unsigned int v2;

    Vector3 normal;

    SHADING_MODE shadingMode = SHADING_MODE::FLAT;
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

    bool Intersection(const Ray& ray, float &t, Vector3& n, float &beta, float &gamma, const ObjectBase ** hitObject, bool shadowCheck = false) const override;

    Vector3 GetCentroid() override;

    void GetBoundingVolumePositions(Vector3 &min, Vector3 &max) override;
    
    std::vector<Face *> faces;

    SHADING_MODE shadingMode = SHADING_MODE::FLAT;
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

    void CreateBVH() override;

    bool Intersection(const Ray& ray, float &t, Vector3& n, float &beta, float &gamma, const ObjectBase ** hitObject, bool shadowCheck = false) const override;

    const Mesh* baseMesh;

    SHADING_MODE shadingMode = SHADING_MODE::FLAT;
private:

};

#endif