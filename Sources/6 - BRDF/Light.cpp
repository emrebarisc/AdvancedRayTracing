/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Light.h"

#include "ObjectBase.h"
#include "Scene.h"

bool Light::ShadowCheck(const Vector3& lightPosition, const Vector3& positionAt) const
{
    float distance = (lightPosition - positionAt).Length();
    Vector3 wi = (lightPosition - positionAt).GetNormalized();
    Vector3 o = positionAt + wi * SHADOW_EPSILON;

    float closestT = 0;
    float beta, gamma;
    Vector3 closestN;
    const ObjectBase *closestObject = nullptr;

    Ray ray(o, wi);
    mainScene->SingleRayTrace(ray, closestT, closestN, beta, gamma, &closestObject, true);

    return closestT > 0 && closestT < distance;
}