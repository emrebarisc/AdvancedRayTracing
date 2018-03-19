#ifndef __MESH_H__
#define __MESH_H__

#include <vector>

#include "ObjectBase.h"
#include "Math.h"

struct Face
{
    Face() : v0(0), v1(0), v2(0), normal(Vector3::ZeroVector())
    {
    
    }

    ~Face()
    {

    }

    bool Intersection(const Vector3& e, const Vector3& d, float &t, bool shadowCheck = false);

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

    bool Intersection(const Vector3& e, const Vector3& d, float &t, bool shadowCheck = false) override;

    std::vector<Face *> faces;

private:

};

#endif