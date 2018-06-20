/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Light.h"

#include "ObjectBase.h"
#include "Scene.h"

#include "LightMesh.h"
#include "LightSphere.h"

bool Light::ShadowCheck(const Vector3& lightPosition, const Vector3& positionAt) const
{
    float distance = (lightPosition - positionAt).Length();
    Vector3 wi = -GetDirection(lightPosition, positionAt);
    Vector3 o = positionAt + wi * SHADOW_EPSILON;

    float closestT = 0;
    float beta, gamma;
    Vector3 closestN;
    const ObjectBase *closestObject = nullptr;

    Ray ray(o, wi);
    mainScene->SingleRayTrace(ray, closestT, closestN, beta, gamma, &closestObject, true);

    bool result = closestT > 0 && closestT < distance;

    if(result && dynamic_cast<const LightMesh*>(closestObject->parentObject))
    {
        return false;
    }

    if(result && dynamic_cast<const LightSphere*>(closestObject))
    {
        return false;
    }

    return result;
}