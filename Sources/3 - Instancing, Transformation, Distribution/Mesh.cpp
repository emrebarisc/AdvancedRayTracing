/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "BoundingVolume.h"
#include "Math.h"
#include "Mesh.h"
#include "Scene.h"

bool Face::Intersection(const Ray &ray, float &t, Vector3& n, bool shadowCheck) const
{
    Vector3 a = mainScene->vertices[v0 - 1];
    Vector3 b = mainScene->vertices[v1 - 1];
    Vector3 c = mainScene->vertices[v2 - 1];

    // Back-face culling
    if(!shadowCheck && Vector3::Dot(ray.dir, normal) >= 0)
    {
        return false;
    }

    Vector3 aMinusB = a - b;
    Vector3 aMinusC = a - c;
    Vector3 aMinusE = a - ray.e;
    
    float detA, detBeta, detGamma;
    Math::Determinant(aMinusB, aMinusC, ray.dir);

    detA = Math::Determinant(aMinusB, aMinusC, ray.dir);

    if(detA == 0)
    {
        return false;
    }

    detBeta = Math::Determinant(aMinusE, aMinusC, ray.dir) / detA;
    detGamma = Math::Determinant(aMinusB, aMinusE, ray.dir) / detA;
    t = Math::Determinant(aMinusB, aMinusC, aMinusE) / detA;

    if (   t > 0
        && 0 <= detBeta
        && 0 <= detGamma
        && detBeta + detGamma <= 1)
    {
        n = this->normal;
        return true;
    }

    return false;
}

Vector3 Face::GetCentroid()
{
    return (mainScene->vertices[v0 - 1] + mainScene->vertices[v1 - 1] + mainScene->vertices[v2 - 1]) / 3;
}

void Face::GetBoundingVolumePositions(Vector3 &min, Vector3 &max)
{
    Vector3 vertex1 = mainScene->vertices[v0 - 1];
    Vector3 vertex2 = mainScene->vertices[v1 - 1];
    Vector3 vertex3 = mainScene->vertices[v2 - 1];

    min.x = vertex1.x;
    min.y = vertex1.y;
    min.z = vertex1.z;

    max.x = vertex1.x;
    max.y = vertex1.y;
    max.z = vertex1.z;

    if(vertex2.x < min.x) min.x = vertex2.x;
    if(vertex2.y < min.y) min.y = vertex2.y;
    if(vertex2.z < min.z) min.z = vertex2.z;

    if(vertex3.x < min.x) min.x = vertex3.x;
    if(vertex3.y < min.y) min.y = vertex3.y;
    if(vertex3.z < min.z) min.z = vertex3.z;

    if(vertex2.x > max.x) max.x = vertex2.x;
    if(vertex2.y > max.y) max.y = vertex2.y;
    if(vertex2.z > max.z) max.z = vertex2.z;

    if(vertex3.x > max.x) max.x = vertex3.x;
    if(vertex3.y > max.y) max.y = vertex3.y;
    if(vertex3.z > max.z) max.z = vertex3.z;
}

bool Mesh::Intersection(const Ray &ray, float& t, Vector3& n, bool shadowCheck) const
{
    unsigned int faceCount = faces.size();

    float outT = MAX_FLOAT;
    bool out = false;

    for(unsigned int faceIndex = 0; faceIndex < faceCount; faceIndex++)
    {
        Face *currFace = faces[faceIndex];

        float iteT;
        Vector3 iteN;
        if(currFace->Intersection(ray, iteT, iteN, shadowCheck))
        {        
            if(outT > iteT && iteT > 0)
            {
                out = true;
                outT = iteT;
                n = currFace->normal;
            }
        }
    }

    t = outT; 
    return out;
}

// Get centeroid position by calling GetBoundingVolume function for now.
// Can be converted to a variable later on.
Vector3 Mesh::GetCentroid()
{
    Vector3 meshMin, meshMax;
    GetBoundingVolumePositions(meshMin, meshMax);

    return Vector3( meshMin.x + ((meshMax.x - meshMin.x) * 0.5f),
                    meshMin.y + ((meshMax.y - meshMin.y) * 0.5f),
                    meshMin.z + ((meshMax.z - meshMin.z) * 0.5f));
}

void Mesh::GetBoundingVolumePositions(Vector3 &min, Vector3 &max)
{
    min = Vector3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
    max = Vector3(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);
    
    size_t faceCount = faces.size();
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
}

void Mesh::CreateBVH()
{
    bvh.CreateBVH(this);
}