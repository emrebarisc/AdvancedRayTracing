/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __LIGHTMESH_H__
#define __LIGHTMESH_H__

#include "PointLight.h"
#include "Mesh.h"

class LightMesh : public Light, public Mesh
{
public:
    LightMesh() : Light(), Mesh()
    {

    }

    ~LightMesh()
    {
        
    }

    bool Intersection(const Ray& ray, float &t, Vector3& n, float &beta, float &gamma, const ObjectBase ** hitObject, bool shadowCheck = false) const override
    {
        return Mesh::Intersection(ray, t, n, beta, gamma, hitObject, shadowCheck);
    }

    Vector3 GetPosition() const override;
    
    Vector3 GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const override;

    //Vector3 radiance;

private:

};

#endif