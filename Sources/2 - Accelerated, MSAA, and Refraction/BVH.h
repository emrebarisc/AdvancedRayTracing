/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __BVH_H__
#define __BVH_H__

#include <vector>

#include "Math.h"

class BoundingVolume;
class Face;
class Mesh;
class ObjectBase;

enum AXIS : unsigned char
{
    X = 0,
    Y,
    Z
};

class BVH
{
public:
    BVH()
    {
        
    }

    ~BVH();

    void DestructorHelper(ObjectBase *obj);

    void CreateBVH(Mesh *mesh);

    BoundingVolume* GetBoundingVolume(const std::vector<Face *> &faces);

    ObjectBase *root;

private:
    ObjectBase* RecursivelySplit(const std::vector<Face *> &faces, AXIS axis, unsigned int recursionDepth = 0);
};

#endif