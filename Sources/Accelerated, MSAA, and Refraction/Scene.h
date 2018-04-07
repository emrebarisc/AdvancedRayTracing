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
#include "BVH.h"

class ObjectBase;

/*
    Scene class containing all the scene data
*/
class Scene
{
public:
    Scene();
    ~Scene();

    void CreateBVH();
    
    // Scene data parser
    void ReadSceneData(char *filePath);

    // Make hitObject null when you do not need collided object's information
    // In shadow check for example 
    // Ray trace throughout the scene and return the result
    bool SingleRayTrace(const Vector3& e, const Vector3& d, float &hitT, Vector3 &hitN, ObjectBase **hitObject = nullptr, bool shadowCheck = false) const;

    bool SingleRayTraceBVH(const Vector3& e, const Vector3& d, float &hitT, Vector3 &hitN, ObjectBase **hitObject = nullptr, bool shadowCheck = false) const;
    bool SingleRayTraceNonBVH(const Vector3& e, const Vector3& d, float &hitT, Vector3 &hitN, ObjectBase **hitObject = nullptr, bool shadowCheck = false) const;
    
    std::vector<Camera> cameras;
    std::vector<PointLight> pointLights;
    std::vector<Material> materials;
    std::vector<Vector3> vertices;
    std::vector<ObjectBase *> objects;
    BVH bvh;

    Vector3 ambientLight;

    Colori bgColor;
    
    unsigned int maxRecursionDepth;

    bool useBVH = true;
};

// Global scene variable
extern Scene *mainScene;

#endif