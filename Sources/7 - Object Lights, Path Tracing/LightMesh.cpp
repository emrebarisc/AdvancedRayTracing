/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "LightMesh.h"

#include "RandomGenerator.h"
#include "Scene.h"

Vector3 LightMesh::GetPosition() const
{
    unsigned int randomValue = RandomGenerator::GetRandomUInt(faces.size());
    
    Face *randomFace = faces[randomValue];

    float randomValue1 = RandomGenerator::GetRandomFloat();
    float randomValue2 = RandomGenerator::GetRandomFloat();
    
    Vector3 A = mainScene->vertices[randomFace->v0 - 1];
    Vector3 B = mainScene->vertices[randomFace->v1 - 1];
    Vector3 C = mainScene->vertices[randomFace->v2 - 1];

    Vector3 q = (1 - randomValue2) * B + randomValue2 * C;
    Vector3 p = (1 - sqrt(randomValue1)) * A + sqrt(randomValue1) * q;

    //p += randomFace->normal * EPSILON * 10;

    return Vector3(inverseTransformationMatrix * Vector4(p, 1.f)) + randomFace->normal * EPSILON * 10;
}

Vector3 LightMesh::GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const
{
    // float distance = (lightPosition - positionAt).Length();
    // return intensity / (distance * distance);

    // We actually return radiance 
    return intensity;
}