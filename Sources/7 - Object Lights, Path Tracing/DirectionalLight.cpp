/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "DirectionalLight.h"

#include "ObjectBase.h"
#include "Scene.h"

Vector3 DirectionalLight::GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const
{
    return radiance;
}

bool DirectionalLight::ShadowCheck(const Vector3& lightPosition, const Vector3& positionAt) const
{
    float closestT = 0;
    float beta, gamma;
    Vector3 closestN;
    const ObjectBase *closestObject = nullptr;

    Ray ray(positionAt - direction * SHADOW_EPSILON, -direction);
    return mainScene->SingleRayTrace(ray, closestT, closestN, beta, gamma, &closestObject, true);
}