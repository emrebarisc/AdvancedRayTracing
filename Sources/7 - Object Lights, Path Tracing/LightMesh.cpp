/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "LightMesh.h"

#include "Scene.h"

void LightMesh::SetAreaLight()
{
    position = Vector3(inverseTransformationMatrix * Vector4(mainScene->vertices[faces[0]->v0 - 1], 1.f));

    edgeVectorU = Vector3(inverseTransformationMatrix * Vector4(mainScene->vertices[faces[0]->v1 - 1], 1.f)) - position;
    edgeVectorV = Vector3(inverseTransformationMatrix * Vector4(mainScene->vertices[faces[0]->v2 - 1], 1.f)) - position;

    lightNormal = Vector3::Cross(edgeVectorV, edgeVectorU);
    lightNormal.Normalize();

    position += lightNormal * EPSILON;
}

Vector3 LightMesh::GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const
{
    //return AreaLight::GetIntensityAtPosition(lightPosition, positionAt);
    // Vector3 l = lightPosition - positionAt;
    // float distance = l.Length();

    return intensity;// * (distance * distance) / Vector3::Dot(l.GetNormalized(), lightNormal);
}