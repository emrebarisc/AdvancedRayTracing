/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "LightMesh.h"

#include <random>

#include "Scene.h"

std::random_device rd_temp;
std::mt19937 rg_temp(rd_temp());
std::uniform_real_distribution<float> ud_temp(0.0, 1.0);

Vector3 LightMesh::GetPosition() const
{
    unsigned int randomValue = (unsigned int)(ud_temp(rg_temp) * faces.size());
    
    Face *randomFace = faces[randomValue];

    float randomValue1 = ud_temp(rg_temp);
    float randomValue2 = ud_temp(rg_temp);
    
    Vector3 A = mainScene->vertices[randomFace->v0 - 1];
    Vector3 B = mainScene->vertices[randomFace->v1 - 1];
    Vector3 C = mainScene->vertices[randomFace->v2 - 1];

    Vector3 q = (1 - randomValue2) * B + randomValue2 * C;
    Vector3 p = (1 - sqrt(randomValue1)) * A + sqrt(randomValue1) * q;
    
    p += randomFace->normal * EPSILON * 10;

    return p;
}

Vector3 LightMesh::GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const
{
    // float distance = (lightPosition - positionAt).Length();
    // return intensity / (distance * distance);

    // We actually return radiance 
    return intensity;
}