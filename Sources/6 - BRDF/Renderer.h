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
    RendererInfo(const Camera *cam) :
        e(cam->position),
        w(cam->gaze),
        u(cam->right),
        v(cam->up),
        l(cam->nearPlane.x),
        r(cam->nearPlane.y),
        b(cam->nearPlane.z),
        t(cam->nearPlane.w),
        distance(cam->nearDistance),
        camera(cam)
    {
        m = e + (w * distance);
        q = m + u * l + v * t;
    }

    Vector3 m, q;
    const Vector3 &e, &w, &u, &v;
    float l, r, b, t; 
    float distance;  
    const Camera *camera;
};

struct ShaderInfo
{
    ShaderInfo(const Ray& r, const ObjectBase* o, const Vector3& ip, const Vector3& sn, float b = 0.f, float g = 0.f) : 
        ray(r),
        shadingObject(o),
        intersectionPoint(ip),
        shapeNormal(sn),
        beta(b),
        gamma(g)
    {
        
    }

    const Ray &ray;
    const ObjectBase* shadingObject;
    const Vector3 &intersectionPoint;
    const Vector3 &shapeNormal;
    float beta;
    float gamma;
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
    static Vector3 CalculateDiffuseShader(const ShaderInfo& shaderInfo, const Vector3 &diffuse, const Vector3 &wi, const Vector3 &lightIntensity);
    
    // Calculate blinn phong color
    static Vector3 CalculateSpecularShader(const ShaderInfo& shaderInfo, const Vector3 &wi, const Vector3 &lightIntensity);

    // Calculate reflection
    static Vector3 CalculateReflection(const ShaderInfo &shaderInfo, unsigned int recursionDepth = 0);

    // Calculate refraction
    static Vector3 CalculateRefraction(const ShaderInfo &shaderInfo, float &schlickValue, unsigned int recursionDepth = 0);

    // Calculate mirror reflection
    static Vector3 CalculateMirrorReflection(const ShaderInfo &si, unsigned int recursionDepth = 0);

    // Calculate transparency
    static Vector3 CalculateTransparency(const ShaderInfo& si, unsigned int recursionDepth = 0);
    
private:
    static void ThreadFunction(Camera *currentCamera, int startX, int startY, int width, int height, /* unsigned char */ float *colorBuffer);
    static Colorf RenderPixel(float x, float y, const RendererInfo &ri);

};

#endif