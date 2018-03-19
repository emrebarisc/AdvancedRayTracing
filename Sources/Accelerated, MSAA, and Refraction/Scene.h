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
#include "Color.h"
#include "Mesh.h"
#include "Sphere.h"

class ObjectBase;

/*
    Scene class containing all the scene data
*/
class Scene
{
public:
    Scene();
    ~Scene()
    {
        unsigned int meshCount = meshes.size();
        for(auto mesh : meshes)
        {
            delete mesh;
        }

        unsigned int sphereCount = spheres.size();
        for(auto sphere : spheres)
        {
            delete sphere;
        }
    }

    // Scene data parser
    void ReadSceneData(char *filePath);

    // Make hitObject null when you do not need collided object's information
    // In shadow check for example 
    // Ray trace throughout the scene and return the result
    bool SingleRayTrace(const Vector3& e, const Vector3& d, float &hitT, Vector3 &hitN, ObjectBase **hitObject = nullptr, bool ShadowCheck = false);
    
    std::vector<Camera> cameras;
    std::vector<PointLight> pointLights;
    std::vector<Material> materials;
    std::vector<Vector3> vertices;
    std::vector<Mesh *> meshes;
    std::vector<Sphere *> spheres;

    Vector3 ambientLight;

    Colori bgColor;
    int maxRecursionDepth;
};

// Global scene variable
extern Scene *mainScene;

#endif