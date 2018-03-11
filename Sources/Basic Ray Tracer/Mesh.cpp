#include "Mesh.h"
#include "Scene.h"
#include "Math.h"

#include <limits>

bool Face::Intersection(const Vector3& e, const Vector3& d, float &t, bool shadowCheck)
{
    Vector3 a = mainScene->vertices[v0 - 1];
    Vector3 b = mainScene->vertices[v1 - 1];
    Vector3 c = mainScene->vertices[v2 - 1];

    if(normal == Vector3(0))
    {
        normal = Vector3::Cross(c - b, a - b);
        Vector3::Normalize(normal);
    }

    if( !shadowCheck && Vector3::Dot(d, normal) > 0)
    {
        return false;
    }

    Vector3 aMinusB = a - b;
    Vector3 aMinusC = a - c;
    Vector3 aMinusE = a - e;

    float detA, detBeta, detGamma;
    Math::Determinant(aMinusB, aMinusC, d);

    detA = Math::Determinant(aMinusB, aMinusC, d);

    if(detA == 0)
    {
        return false;
    }

    detBeta = Math::Determinant(aMinusE, aMinusC, d) / detA;
    detGamma = Math::Determinant(aMinusB, aMinusE, d) / detA;
    t = Math::Determinant(aMinusB, aMinusC, aMinusE) / detA;

    Vector3 intersectionPoint = e + d * t;

    if (   t > EPSILON
        && 0 <= detBeta
        && 0 <= detGamma
        && detBeta + detGamma <= 1)
    {
        return true;
    }

    return false;
}

bool Mesh::Intersection(const Vector3& e, const Vector3& d, float& t, bool shadowCheck)
{
    unsigned int faceCount = faces.size();

    float outT = std::numeric_limits<float>::max();
    bool out;

    for(unsigned int faceIndex = 0; faceIndex < faceCount; faceIndex++)
    {
        Face *currFace = &faces[faceIndex];

        float iteT;
        if(currFace->Intersection(e, d, iteT))
        {        
            if(outT > iteT && iteT > 0)
            {
                out = true;
                outT = iteT;
            }
        }
    }

    t = outT; 
    return out;
}