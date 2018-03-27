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
    sampleAmount = 1;
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

bool Scene::SingleRayTrace(const Vector3& e, const Vector3& d, float &hitT, Vector3 &hitN, ObjectBase **hitObject, bool shadowCheck) const
{
    if(useBVH)
    {
        return SingleRayTraceBVH(e, d, hitT, hitN, hitObject, shadowCheck);
    }
    else
    {
        return SingleRayTraceNonBVH(e, d, hitT, hitN, hitObject, shadowCheck);
    }
}

bool Scene::SingleRayTraceBVH(const Vector3& e, const Vector3& d, float &hitT, Vector3 &hitN, ObjectBase **hitObject, bool shadowCheck) const
{
    if(hitObject != nullptr) *hitObject = nullptr;
    hitT = 0;

    size_t objectCount = objects.size();
    for(size_t objectIndex = 0; objectIndex < objectCount; objectIndex++)
    {
        float t;
        Vector3 n;
        if(objects[objectIndex]->bvh.root->Intersection(e, d, t, n, shadowCheck))
        {
            if ((hitT > 0 && hitT > t) || hitT <= 0)
            {
                hitT = t;
                hitN = n;

                *hitObject = objects[objectIndex];
            }
        }
    }

    return hitT > 0 ? true : false;
}

bool Scene::SingleRayTraceNonBVH(const Vector3& e, const Vector3& d, float &hitT, Vector3 &hitN, ObjectBase **hitObject, bool shadowCheck) const
{
    unsigned int objectCount = objects.size();

    if(hitObject != nullptr) *hitObject = nullptr;
    hitT = 0;

    for (size_t objectIndex = 0; objectIndex < objectCount; objectIndex++)
	{
		ObjectBase *currentObject = objects[objectIndex];
		
        float t;
        Vector3 n;

		if (currentObject->Intersection(e, d, t, n, shadowCheck))
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