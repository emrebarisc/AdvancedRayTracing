/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>

#include "Math.h"
#include "Camera.h"
#include "PointLight.h"
#include "Material.h"
#include "ObjectBase.h"
#include "Color.h"
#include "Mesh.h"
#include "Sphere.h"

class ObjectBase;

class Scene
{
public:
    Scene();
    ~Scene()
    {

    }

    void ReadSceneData(char *filePath);
    
    std::vector<Camera> cameras;
    std::vector<PointLight> pointLights;
    std::vector<Material> materials;
    std::vector<ObjectBase> objects;
    std::vector<Vector3> vertices;
    std::vector<Mesh> meshes;
    std::vector<Sphere> spheres;

    Colori ambientLight;

    Vector3 bgColor;
    int maxRecursionDepth;
};

extern Scene *mainScene;

#endif