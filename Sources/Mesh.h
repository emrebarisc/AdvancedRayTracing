#ifndef __MESH_H__
#define __MESH_H__

#include <vector>

#include "ObjectBase.h"
#include "Math.h"

struct Face
{
    bool Intersection(const Vector3& e, const Vector3& d, float &t);

    unsigned int v0;
    unsigned int v1;
    unsigned int v2;

    Vector3 normal;
    
};

class Mesh : public ObjectBase
{
public:
    Mesh();
    ~Mesh();

    bool Intersection(const Vector3& e, const Vector3& d, float &t) override;

    std::vector<Face> faces;

private:

};

#endif