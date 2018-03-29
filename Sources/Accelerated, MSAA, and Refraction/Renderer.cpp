/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Renderer.h"

#include <mutex>
#include <thread>
#include <random>

#include "Scene.h"
#include "OutputManager.h"
#include "Material.h"
#include "ObjectBase.h"

std::mutex mut;

#define MAX_THREAD_COUNT 8

#define GAUSSIAN_VALUE(x, y) ((1 / TWO_PI) * (pow(NATURAL_LOGARITHM, -((x * x + y * y) * 0.5f))))

int imageWidth, imageHeight;

void Renderer::RenderScene()
{
    int cameraCount = mainScene->cameras.size();
    for(int cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
    {
        Camera *currentCamera = &mainScene->cameras[cameraIndex];

        imageWidth = currentCamera->imageWidth;
        imageHeight = currentCamera->imageHeight;

        unsigned char *image = new unsigned char[imageWidth * imageHeight * 3];
        
        std::thread threads[MAX_THREAD_COUNT + 1];

        int startingX = 0, startingY = 0;

        int threadedHeight = imageHeight / MAX_THREAD_COUNT;
        int residualHeight = imageHeight - (threadedHeight * MAX_THREAD_COUNT);

        for(unsigned int i = 0; i < MAX_THREAD_COUNT; i++)
        {
            threads[i] = std::thread(&Renderer::ThreadFunction, currentCamera, startingX, startingY, imageWidth, threadedHeight, image);
            startingY += threadedHeight;
        }
        
        threads[MAX_THREAD_COUNT] = std::thread(&Renderer::ThreadFunction, currentCamera, startingX, startingY, imageWidth, residualHeight, image);
        
        // Wait all the threads to join including residual thread
        for(unsigned int i = 0; i <= MAX_THREAD_COUNT; i++)
        {
            threads[i].join();
        }

        OutputManager::WritePng(currentCamera->imageName.c_str(), imageWidth, imageHeight, image);
        //OutputManager::WritePpm(currentCamera->imageName.c_str(), imageWidth, imageHeight, image);
    }
}

void Renderer::ThreadFunction(Camera *currentCamera, int startX, int startY, int width, int height, unsigned char *colorBuffer)
{
    if(height == 0) return;

    RendererInfo ri;

    ri.e = currentCamera->position;
    ri.w = currentCamera->gaze;
    ri.v = currentCamera->up;
    ri.u = currentCamera->right;

    ri.l = currentCamera->nearPlane.x;
    ri.r = currentCamera->nearPlane.y;
    ri.b = currentCamera->nearPlane.z;
    ri.t = currentCamera->nearPlane.w;

    ri.distance = currentCamera->nearDistance;

    ri.m = ri.e + (ri.w * ri.distance);
    ri.q = ri.m + ri.u * ri.l + ri.v * ri.t;

    float su, sv;
    Vector3 s, ray, d;

    int pixelIndex = 3 * (startY * width + startX);

    unsigned int endX = startX + width;
    unsigned int endY = startY + height;

    std::random_device randomDevice;  //Will be used to obtain a seed for the random number engine
    std::mt19937 randomGenerator(randomDevice()); //Standard mersenne_twister_engine seeded with randomDevice()
    std::uniform_real_distribution<float> uniformDistribution(0.0, 1.0);

    for(int y = startY; y < endY; y++)
    {
        for(int x = startX; x < endX; x++)
        {
            Colori pixelColor = RenderPixel(x + 0.5f, y + 0.5f, ri);

            float divider = 1.f;
            if(mainScene->sampleAmount > 1)
            {
                int pAmount = mainScene->sampleAmount;
                int qAmount = mainScene->sampleAmount;
                for(int pSample = 0; pSample < pAmount; pSample++)
                {
                    for(int qSample = 0; qSample < qAmount; qSample++)
                    {
                        float randomU = uniformDistribution(randomGenerator);
                        float randomV = uniformDistribution(randomGenerator);

                        float gaussianValue = GAUSSIAN_VALUE((pSample + randomU) / pAmount, (qSample + randomV) / qAmount);

                        Colori color = gaussianValue * RenderPixel(x + ((pSample + randomU) / pAmount) , y + ((qSample + randomV) / qAmount), ri);
                        divider += gaussianValue;
                        pixelColor += color;
                    }
                }
            }
            pixelColor /= divider;
            pixelColor.ClampColor(0, 255);

            colorBuffer[pixelIndex++] = pixelColor.r;
            colorBuffer[pixelIndex++] = pixelColor.g;
            colorBuffer[pixelIndex++] = pixelColor.b;
        }
    }
}

Colori Renderer::RenderPixel(float x, float y, const RendererInfo &ri)
{
    float su = (ri.r - ri.l) * x / imageWidth;
    float sv = (ri.t - ri.b) * y / imageHeight;

    Vector3 s = ri.q + (ri.u * su) - (ri.v * sv);
    Vector3 d = s - ri.e;
    Vector3::Normalize(d);
    Vector3 ray = ri.e + d;

    float closestT = -1;
    Vector3 closestN = Vector3::ZeroVector();
    ObjectBase *closestObject = nullptr;

    Colori pixelColor;

    if(mainScene->SingleRayTrace(ri.e, d, closestT, closestN, &closestObject))
    {
        Vector3 intersectionPoint = ri.e + d * closestT;

        ShaderInfo si(closestObject->material, ri.e, intersectionPoint, closestN);

        pixelColor = Colori(CalculateShader(si, d, REFRACTION_STAT::ENTERING));
    }
    else
    {
        pixelColor = mainScene->bgColor;
    }

    return pixelColor;
}

Vector3 Renderer::CalculateShader(const ShaderInfo &si, Vector3 d, REFRACTION_STAT rs, int recursionDepth)
{
    unsigned int lightCount = mainScene->pointLights.size();

    Vector3 pixelColor = CalculateAmbientShader(si.material->ambient, mainScene->ambientLight);

    for(unsigned int lightIndex = 0; lightIndex < lightCount; lightIndex++)
    {
        const PointLight *currentPointLight = &mainScene->pointLights[lightIndex];

        if(si.material->mirror != Vector3::ZeroVector())
        {
            pixelColor += CalculateMirrorReflectance(si, d, rs, recursionDepth);
        }

        if(si.material->transparency != Vector3::ZeroVector() && recursionDepth < mainScene->maxRecursionDepth)
        {
            pixelColor += CalculateTransparency(si, d, rs, recursionDepth);
        }

        // If the intersection point is in a shadow area, then don't make further calculations
        if (ShadowCheck(si.intersectionPoint, currentPointLight->position))
        {
            continue;
        }

        pixelColor += CalculateDiffuseShader(si.material->diffuse, currentPointLight->intensity, currentPointLight->position, si.intersectionPoint, si.shapeNormal);
        pixelColor += CalculateBlinnPhongShader(si.material->specular, si.material->phongExponent, currentPointLight->intensity, currentPointLight->position, si.camPos, si.intersectionPoint, si.shapeNormal);
    }

    return pixelColor;
}

Vector3 Renderer::CalculateAmbientShader(const Vector3& ambientReflectance, const Vector3& intensity)
{
  return ambientReflectance * intensity;
}

Vector3 Renderer::CalculateDiffuseShader(const Vector3& diffuseReflectance, const Vector3& intensity, const Vector3& lightPos, const Vector3& intersectionPoint, const Vector3& shapeNormal)
{
  float distance = (intersectionPoint - lightPos).Length();

  Vector3 iOverRSquare = intensity / (distance * distance);
  Vector3 wi = lightPos - intersectionPoint;
  wi /= wi.Length();
  float cosTethaPrime = mathMax(0, Vector3::Dot(wi, shapeNormal));

  return diffuseReflectance * cosTethaPrime * iOverRSquare;
}

Vector3 Renderer::CalculateBlinnPhongShader(const Vector3& specularReflectance, float phongExponent, const Vector3& intensity, const Vector3& lightPos, const Vector3& camPos, const Vector3& intersectionPoint, const Vector3& shapeNormal)
{
  float distance = (intersectionPoint - lightPos).Length();

  Vector3 wi = lightPos - intersectionPoint;
  wi /= wi.Length();
  Vector3 wo = camPos - intersectionPoint;
  wo /= wo.Length();
  Vector3 h = (wi + wo) / (wi + wo).Length();

  Vector3 incomingRadiance = intensity / (distance * distance);
  float cosAlphaPrime = mathMax(0, Vector3::Dot(shapeNormal, h));

  return specularReflectance * std::pow(cosAlphaPrime, phongExponent) * incomingRadiance;
}

Vector3 Renderer::CalculateMirrorReflectance(const ShaderInfo &si, const Vector3 &d, REFRACTION_STAT rs, unsigned int recursionDepth)
{
    Vector3 wo = si.camPos - si.intersectionPoint;
    Vector3::Normalize(wo);
    Vector3 wr = -wo + 2 * si.shapeNormal * Vector3::Dot(si.shapeNormal, wo);
    Vector3::Normalize(wr);

    Vector3 o = si.intersectionPoint + (wr * MIRROR_EPSILON);

    float closestT;
    Vector3 closestN;
    ObjectBase* closestObject;
    if(mainScene->SingleRayTrace(o, wr, closestT, closestN, &closestObject))
    {
        if(recursionDepth < mainScene->maxRecursionDepth)
        {
            Vector3 newIntersectionPoint = o + wr * closestT;

            ShaderInfo newSi(closestObject->material, si.intersectionPoint, newIntersectionPoint, closestN);

            return si.material->mirror * CalculateShader(newSi, d, rs, ++recursionDepth);
        }
    }

    return Vector3(0.f);
}

Vector3 Renderer::CalculateTransparency(const ShaderInfo& shaderInfo, const Vector3& d, REFRACTION_STAT refractionStat, unsigned int recursionDepth)
{
    float tetha, cosTetha, delta;
    Vector3 t;

    Vector3 direction = -d;

    Vector3 normal;
    float fractionDivision;

    if(refractionStat == REFRACTION_STAT::LEAVING)
    {
        normal = -shaderInfo.shapeNormal;
        fractionDivision = shaderInfo.material->refractionIndex;
    }
    else
    {
        normal = shaderInfo.shapeNormal;
        fractionDivision = 1 / shaderInfo.material->refractionIndex;
    }

    cosTetha = Vector3::Dot(direction, normal);
    delta = 1 - pow(fractionDivision, 2) * (1 - pow(cosTetha, 2));

    if(refractionStat == REFRACTION_STAT::LEAVING && delta < 0)
    {
        float R0 = pow(((shaderInfo.material->refractionIndex - 1) * 0.5f), 2);

        float rTetha = R0 + (1 - R0) * pow(1 - cosTetha, 5);
        return Vector3(0);
    }
    
    float cosPhi = sqrt(delta);
    t = (direction - normal * cosTetha) * fractionDivision - normal * cosPhi;
    Vector3::Normalize(t);

    float hitT;
    Vector3 hitN;
    ObjectBase *hitObject;
    
    Vector3 o = shaderInfo.intersectionPoint;// + t * EPSILON;

    if(mainScene->SingleRayTrace(o, t, hitT, hitN, &hitObject))
    {
        Vector3 nextIntersectionPoint = shaderInfo.intersectionPoint + hitT * t;
        ShaderInfo reflectedShaderInfo(hitObject->material, shaderInfo.intersectionPoint, nextIntersectionPoint, hitN);

        return shaderInfo.material->transparency * CalculateShader(reflectedShaderInfo, t, refractionStat == REFRACTION_STAT::LEAVING ? REFRACTION_STAT::ENTERING : REFRACTION_STAT::LEAVING, ++recursionDepth);
    }
    
    return Vector3::ZeroVector();
}

bool Renderer::ShadowCheck(const Vector3 &intersectionPoint, const Vector3 &lightPos)
{
    float distance = (lightPos - intersectionPoint).Length();
    Vector3 wi = (lightPos - intersectionPoint).GetNormalized();
    Vector3 o = intersectionPoint + wi * EPSILON;

    float closestT = 0;
    Vector3 closestN;
    ObjectBase *closestObject = nullptr;
    mainScene->SingleRayTrace(o, wi, closestT, closestN, &closestObject, true);

    return closestT > 0 && closestT < distance;
}