/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "BVH.h"

#include <vector>

#include "BoundingVolume.h"
#include "Mesh.h"

BVH::~BVH()
{
    DestructorHelper(root);
}

void BVH::DestructorHelper(ObjectBase *obj)
{
    // if(!obj) return;

    // BoundingVolume *bv = dynamic_cast<BoundingVolume *>(obj);

    // if(bv)
    // {
    //     DestructorHelper(bv->left);
    //     DestructorHelper(bv->right);
        
    //     delete bv;
    // }
}

void BVH::CreateBVH(Mesh *mesh)
{
    mesh->bvh.root = RecursivelySplit(mesh->faces, AXIS::X);
}

BoundingVolume *bv;

BoundingVolume* BVH::GetBoundingVolume(const std::vector<Face *> &faces)
{
    size_t const faceCount = faces.size();

    Vector3 min = Vector3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
    Vector3 max = Vector3(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);
    
    for(size_t faceIndex = 0; faceIndex < faceCount; faceIndex++)
    {
        Face* currentFace = faces[faceIndex];
        
        Vector3 faceMin, faceMax;
        currentFace->GetBoundingVolumePositions(faceMin, faceMax);

        if(faceMin.x < min.x) min.x = faceMin.x;
        if(faceMin.y < min.y) min.y = faceMin.y;
        if(faceMin.z < min.z) min.z = faceMin.z;
        
        if(faceMax.x > max.x) max.x = faceMax.x;
        if(faceMax.y > max.y) max.y = faceMax.y;
        if(faceMax.z > max.z) max.z = faceMax.z;
    }

    bv = new BoundingVolume();
    bv->min = min;
    bv->max = max;

    return bv;
}

ObjectBase* BVH::RecursivelySplit(const std::vector<Face *> &faces, AXIS axis)
{
    size_t const faceCount = faces.size();

    if(faceCount == 0)
    {
        return nullptr;
    }
    else if(faceCount == 1)
    {
        return faces[0];
    }
    else if(faceCount == 2)
    {
        BoundingVolume *bv = GetBoundingVolume(faces);
        bv->left = faces[0];
        bv->right = faces[1];
        return bv;
    }

    BoundingVolume *bv = GetBoundingVolume(faces);

    Vector3 centroid = bv->GetCentroid();

    std::vector<Face *> left;
    std::vector<Face *> right;

    size_t i;
    for(i = 0; i < faceCount; i++)
    {
        Face *face = faces[i];
        Vector3 faceCenter = face->GetCentroid();

        if(axis == AXIS::X)
        {
            if(faceCenter.x < centroid.x)
            {
                left.push_back(face);
            }
            else
            {
                right.push_back(face);
            }
        }
        else if(axis == AXIS::Y)
        {
            if(faceCenter.y < centroid.y)
            {
                left.push_back(face);
            }
            else
            {
                right.push_back(face);
            }
        }
        else if(axis == AXIS::Z)
        {
            if(faceCenter.z < centroid.z)
            {
                left.push_back(face);
            }
            else
            {
                right.push_back(face);
            }
        }
    }

    bv->left = RecursivelySplit(left, axis == AXIS::X ? AXIS::Y : 
                                      axis == AXIS::Y ? AXIS::Z : 
                                              AXIS::X);

    bv->right = RecursivelySplit(right,  axis == AXIS::X ? AXIS::Y : 
                                         axis == AXIS::Y ? AXIS::Z : 
                                                 AXIS::X);

    return bv;
}