/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Scene.h"

#include <iostream>

#include "ObjectBase.h"
#include "SceneParser.h"

Scene *mainScene = nullptr;

Scene::Scene()
{
    mainScene = this;
    useBVH = true;
}

Scene::~Scene()
{
    for(auto object : objects)
    {
        delete object;
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

bool Scene::SingleRayTrace(const Ray &ray, float &hitT, Vector3 &hitN, ObjectBase **hitObject, bool shadowCheck) const
{
    if(useBVH)
    {
        return SingleRayTraceBVH(ray, hitT, hitN, hitObject, shadowCheck);
    }
    else
    {
        return SingleRayTraceNonBVH(ray, hitT, hitN, hitObject, shadowCheck);
    }
}

bool Scene::SingleRayTraceBVH(const Ray &ray, float &hitT, Vector3 &hitN, ObjectBase **hitObject, bool shadowCheck) const
{
    if(hitObject != nullptr) *hitObject = nullptr;
    hitT = 0;

    for(auto object : objects)
    {
        float t;
        Vector3 n;

        Vector3 transformatedE = Vector3(object->inverseTransformationMatrix * Vector4(ray.e, 1.f));
        Vector3 transformatedDir = Vector3(object->inverseTransformationMatrix * Vector4(ray.dir, 0.f));

        if(object->bvh.root->Intersection(Ray(transformatedE, transformatedDir), t, n, shadowCheck))
        {
            if ((hitT > 0 && hitT > t) || hitT <= 0)
            {
                hitT = t;
                hitN = n;

                *hitObject = object;
            }
        }
    }

    return hitT > 0 ? true : false;
}

bool Scene::SingleRayTraceNonBVH(const Ray &ray, float &hitT, Vector3 &hitN, ObjectBase **hitObject, bool shadowCheck) const
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

		if (currentObject->Intersection(Ray(transformatedE, transformatedDir), t, n, shadowCheck))
		{
			if ((hitT > 0 && hitT > t) || hitT <= 0)
			{
                hitT = t;
                hitN = n;
                *hitObject = currentObject;
			}
		}
	}

    return hitT > 0 ? true : false;
}