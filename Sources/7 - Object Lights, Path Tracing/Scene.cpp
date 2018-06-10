/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Scene.h"

#include <iostream>

#include "ObjectBase.h"
#include "SceneParser.h"

#include "LightMesh.h"
#include "LightSphere.h"

Scene *mainScene = nullptr;

Scene::Scene()
{
    mainScene = this;
    useBVH = true;

    ambientLight = Vector3::ZeroVector;

    pathTracingBounceCount = 16;
}

Scene::~Scene()
{
    for(auto object : objects)
    {
        delete object;
    }

    for(auto light : lights)
    {
        if(!dynamic_cast<const LightMesh *>(light) && !dynamic_cast<const LightSphere *>(light))
            delete light;
    }

    for(auto texture : textures)
    {
        delete texture;
    }
}

void Scene::CreateBVH()
{
    for(auto object : objects)
    {
        object->CreateBVH();
    }
}

void Scene::ReadSceneData(char *filePath)
{
    SceneParser::Parse(this, filePath);
}

bool Scene::SingleRayTrace(const Ray &ray, float &hitT, Vector3 &hitN, float &beta, float &gamma, const ObjectBase **hitObject, bool shadowCheck) const
{
    if(useBVH)
    {
        return SingleRayTraceBVH(ray, hitT, hitN, beta, gamma, hitObject, shadowCheck);
    }
    else
    {
        return SingleRayTraceNonBVH(ray, hitT, hitN, beta, gamma, hitObject, shadowCheck);
    }
}

bool Scene::SingleRayTraceBVH(const Ray &ray, float &hitT, Vector3 &hitN, float &beta, float &gamma, const ObjectBase **hitObject, bool shadowCheck) const
{
    if(hitObject != nullptr) *hitObject = nullptr;
    hitT = 0;

    for(auto object : objects)
    {
        float t = 0.f, b = 0.f, g = 0.f;
        Vector3 n = Vector3::ZeroVector;
        const ObjectBase *obj = nullptr;

        Vector3 transformatedE = Vector3(object->inverseTransformationMatrix * Vector4(ray.e, 1.f));
        Vector3 transformatedDir = Vector3(object->inverseTransformationMatrix * Vector4(ray.dir, 0.f));

        if(object->bvh.Intersection(Ray(transformatedE, transformatedDir), t, n, b, g, &obj, shadowCheck))
        {
            if ((hitT > 0 && hitT > t) || hitT <= 0)
            {
                hitT = t;
                hitN = n;
                beta = b;
                gamma = g;
                *hitObject = obj;
            }
        }
    }

    return hitT > 0 ? true : false;
}

bool Scene::SingleRayTraceNonBVH(const Ray &ray, float &hitT, Vector3 &hitN, float &beta, float &gamma, const ObjectBase **hitObject, bool shadowCheck) const
{
    unsigned int objectCount = objects.size();

    if(hitObject != nullptr) *hitObject = nullptr;
    hitT = 0;

    for (size_t objectIndex = 0; objectIndex < objectCount; objectIndex++)
	{
		ObjectBase *currentObject = objects[objectIndex];
		
        float t;
        Vector3 n;

        Vector3 transformatedE = Vector3(currentObject->inverseTransformationMatrix * Vector4(ray.e, 1.f));
        Vector3 transformatedDir = Vector3(currentObject->inverseTransformationMatrix * Vector4(ray.dir, 0.f));

		if (currentObject->Intersection(Ray(transformatedE, transformatedDir), t, n, beta, gamma, hitObject, shadowCheck))
		{
			if ((hitT > 0 && hitT > t) || hitT <= 0)
			{
                hitT = t;
                hitN = n;
			}
		}
	}

    return hitT > 0 ? true : false;
}