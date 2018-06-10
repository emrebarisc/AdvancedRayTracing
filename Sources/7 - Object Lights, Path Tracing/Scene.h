/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __SCENE_H__
#define __SCENE_H__

#include <vector>

#include "BVH.h"
#include "Camera.h"
#include "Color.h"
#include "Material.h"
#include "Math.h"
#include "Light.h"
#include "Ray.h"
#include "Texture.h"

class BRDF;
class ObjectBase;
class Texture;

enum class INTEGRATOR : uint8_t
{
    RAY_TRACER = 0,
    PATH_TRACER
};

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
    bool SingleRayTrace(const Ray &ray, float &hitT, Vector3 &hitN, float &beta, float &gamma, const ObjectBase **hitObject = nullptr, bool shadowCheck = false) const;

    bool SingleRayTraceBVH(const Ray &ray, float &hitT, Vector3 &hitN, float &beta, float &gamma, const ObjectBase **hitObject = nullptr, bool shadowCheck = false) const;
    bool SingleRayTraceNonBVH(const Ray &ray, float &hitT, Vector3 &hitN, float &beta, float &gamma, const ObjectBase **hitObject = nullptr, bool shadowCheck = false) const;
    
    std::vector<Camera> cameras;
    std::vector<Light *> lights;
    std::vector<Material> materials;
    std::vector<BRDF *> BRDFs;
    std::vector<Vector3> vertices;
    std::vector<Vector3> vertexNormals;
    std::vector<Vector2i> textureCoordinates;
    std::vector<Texture *> textures;
    std::vector<Vector3> translations;
    std::vector<Vector3> scalings;
    std::vector<Vector4> rotations;
    std::vector<ObjectBase *> objects;
    BVH bvh;

    Vector3 ambientLight;

    Colori bgColor;
    
    unsigned int maxRecursionDepth;

    unsigned int pathTracingBounceCount;

    INTEGRATOR integrator;

    bool useBVH = true;
};

// Global scene variable
extern Scene *mainScene;

#endif