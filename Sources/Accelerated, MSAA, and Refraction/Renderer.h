/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Color.h"
#include "Math.h"
#include "Ray.h"

class Camera;
class ObjectBase;

struct RendererInfo
{
    Vector3 m, q;
    Vector3 e, w, u, v;
    float l, r, b, t;
    float distance;   
};

struct ShaderInfo
{
    ShaderInfo() :
        ray(Ray()),
        shadingObject(nullptr),
        intersectionPoint(Vector3::ZeroVector()),
        shapeNormal(Vector3::ZeroVector())
    {

    }

    ShaderInfo(const Ray& r, ObjectBase* o, const Vector3& ip, const Vector3& sn) : 
        ray(r),
        shadingObject(o),
        intersectionPoint(ip),
        shapeNormal(sn)
    {

    }

    Ray ray;
    ObjectBase* shadingObject;
    Vector3 intersectionPoint;
    Vector3 shapeNormal;
};

enum REFRACTION_STAT : unsigned char
{
    ENTERING = 0,
    LEAVING
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
    static Vector3 CalculateShader(const Vector3 &e, const Vector3 &dir, const ShaderInfo &si, int recursionDepth = 0);
    
    // Calculate ambient color
    static Vector3 CalculateAmbientShader(const Vector3& ambientReflectance, const Vector3& intensity);
    
    // Calculate diffuse color
    static Vector3 CalculateDiffuseShader(const Vector3& diffuseReflectance, const Vector3& intensity, const Vector3& lightPos, const Vector3& intersectionPoint, const Vector3& shapeNormal);
    
    // Calculate blinn phong color
    static Vector3 CalculateBlinnPhongShader(const Vector3& specularReflectance, float phongExponent, const Vector3& intensity, const Vector3& lightPos, const Vector3& camPos, const Vector3& intersectionPoint, const Vector3& shapeNormal);

    // Calculate mirror reflectance
    static Vector3 CalculateMirrorReflectance(const Vector3 &e, const Vector3 &dir, const ShaderInfo &si, unsigned int recursionDepth);

    // Calculate transparency
    static Vector3 CalculateTransparency(const Vector3 &e, const Vector3 &dir, const ShaderInfo& si, unsigned int recursionDepth);
    
    // Check whether an intersecting point is in a shadow or not
    static bool ShadowCheck(const Vector3 & intersectionPoint, const Vector3 &lightPos);

private:
    static void ThreadFunction(Camera *currentCamera, int startX, int startY, int width, int height, unsigned char *colorBuffer);
    static Colori RenderPixel(float x, float y, const RendererInfo &ri);

};

#endif