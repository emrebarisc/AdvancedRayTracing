/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>

#include "Math.h"

#define SHADOW_RAY_EPSILON 0.001f;

class Camera;
class Light;
class Material;
class ObjectBase;

class Scene
{
public:
    Scene();
    ~Scene();

    void ReadSceneData(char *filePath);

private:
    std::vector<Camera *> cameras;
    std::vector<Light *> lights;
    std::vector<Material *> materials;
    std::vector<ObjectBase *> objects;

    Vector3 bgColor;
    int maxRecursionDepth;

};

#endif