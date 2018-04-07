/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Camera.h"
#include "Color.h"
#include "Math.h"
#include "Ray.h"

class Light;
class ObjectBase;

struct RendererInfo
{
    RendererInfo(const Camera *camera) :
        e(camera->position),
        w(camera->gaze),
        u(camera->right),
        v(camera->up),
        l(camera->nearPlane.x),
        r(camera->nearPlane.y),
        b(camera->nearPlane.z),
        t(camera->nearPlane.w),
        distance(camera->nearDistance)
    {
        m = e + (w * distance);
        q = m + u * l + v * t;
    }

    Vector3 m, q;
    const Vector3 &e, &w, &u, &v;
    float l, r, b, t;
    float distance;   
};

struct ShaderInfo
{
    ShaderInfo(const Ray& r, ObjectBase* o, const Vector3& ip, const Vector3& sn) : 
        ray(r),
        shadingObject(o),
        intersectionPoint(ip),
        shapeNormal(sn)
    {
        
    }

    const Ray &ray;
    ObjectBase* shadingObject;
    const Vector3 &intersectionPoint;
    const Vector3 &shapeNormal;
};

/*
    Static renderer class
    Deals with rendering operations
*/
class Renderer
{
public:
    Renderer();
    ~Renderer();

    // Renderer function
    static void RenderScene();

    // Calculates pixel color recursively
    static Vector3 CalculateShader(const ShaderInfo &si, int recursionDepth = 0);
    
    // Calculate ambient color
    static Vector3 CalculateAmbientShader(const Vector3& ambientReflection, const Vector3& intensity);
    
    // Calculate diffuse color
    static Vector3 CalculateDiffuseShader(const ShaderInfo& shaderInfo, const Light* light = nullptr);
    
    // Calculate blinn phong color
    static Vector3 CalculateBlinnPhongShader(const ShaderInfo& shaderInfo, const Light* light = nullptr);

    // Calculate reflection
    static Vector3 CalculateReflection(const ShaderInfo &shaderInfo, unsigned int recursionDepth = 0);

    // Calculate refraction
    static Vector3 CalculateRefraction(const ShaderInfo &shaderInfo, float &schlickValue, unsigned int recursionDepth = 0);

    // Calculate mirror reflection
    static Vector3 CalculateMirrorReflection(const ShaderInfo &si, unsigned int recursionDepth = 0);

    // Calculate transparency
    static Vector3 CalculateTransparency(const ShaderInfo& si, unsigned int recursionDepth = 0);
    
    // Check whether an intersecting point is in a shadow or not
    static bool ShadowCheck(const Vector3 & intersectionPoint, const Vector3 &lightPos);

private:
    static void ThreadFunction(Camera *currentCamera, int startX, int startY, int width, int height, unsigned char *colorBuffer);
    static Colori RenderPixel(float x, float y, const RendererInfo &ri);

};

#endif