/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Color.h"
#include "Math.h"

class Camera;
class Material;

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
        material(nullptr),
        camPos(Vector3::ZeroVector()),
        intersectionPoint(Vector3::ZeroVector()),
        shapeNormal(Vector3::ZeroVector())
    {

    }

    ShaderInfo(const Material* m, const Vector3& cam, const Vector3& ip, const Vector3& sn) : 
        material(m),
        camPos(cam),
        intersectionPoint(ip),
        shapeNormal(sn)
    {

    }

    const Material* material;
    const Vector3& camPos;
    const Vector3& intersectionPoint;
    const Vector3& shapeNormal;
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
    static Vector3 CalculateShader(const ShaderInfo &si, Vector3 d, REFRACTION_STAT rf, int recursionDepth = 0);
    
    // Calculate ambient color
    static Vector3 CalculateAmbientShader(const Vector3& ambientReflectance, const Vector3& intensity);
    
    // Calculate diffuse color
    static Vector3 CalculateDiffuseShader(const Vector3& diffuseReflectance, const Vector3& intensity, const Vector3& lightPos, const Vector3& intersectionPoint, const Vector3& shapeNormal);
    
    // Calculate blinn phong color
    static Vector3 CalculateBlinnPhongShader(const Vector3& specularReflectance, float phongExponent, const Vector3& intensity, const Vector3& lightPos, const Vector3& camPos, const Vector3& intersectionPoint, const Vector3& shapeNormal);

    // Calculate mirror reflectance
    static Vector3 CalculateMirrorReflectance(const ShaderInfo &si, const Vector3 &d, REFRACTION_STAT rs, unsigned int recursionDepth);

    // Calculate transparency
    static Vector3 CalculateTransparency(const ShaderInfo& si, const Vector3& direction, REFRACTION_STAT refractionStat, unsigned int recursionDepth);
    
    // Check whether an intersecting point is in a shadow or not
    static bool ShadowCheck(const Vector3 & intersectionPoint, const Vector3 &lightPos);

private:
    static void ThreadFunction(Camera *currentCamera, int startX, int startY, int width, int height, unsigned char *colorBuffer);
    static Colori RenderPixel(float x, float y, const RendererInfo &ri);

};

#endif