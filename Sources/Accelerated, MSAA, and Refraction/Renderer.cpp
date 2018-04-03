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
            if(currentCamera->numberOfSamples > 1)
            {
                int sqrtSampleAmount = sqrt(currentCamera->numberOfSamples);
                int pAmount = sqrtSampleAmount;
                int qAmount = sqrtSampleAmount;
                for(int pSample = 0; pSample < pAmount; pSample++)
                {
                    for(int qSample = 0; qSample < qAmount; qSample++)
                    {
                        float randomU = uniformDistribution(randomGenerator);
                        float randomV = uniformDistribution(randomGenerator);

                        float gaussianValue = GAUSSIAN_VALUE((pSample + randomU) / pAmount, (qSample + randomV) / qAmount);

                        pixelColor += gaussianValue * RenderPixel(x + ((pSample + randomU) / pAmount) , y + ((qSample + randomV) / qAmount), ri);
                        divider += gaussianValue;
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

        ShaderInfo si(Ray(ri.e, d), closestObject, intersectionPoint, closestN);

        pixelColor = Colori(CalculateShader(ri.e, d, si));
    }
    else
    {
        pixelColor = mainScene->bgColor;
    }

    return pixelColor;
}

// e and dir are temporary fixes
Vector3 Renderer::CalculateShader(const Vector3 &e, const Vector3 &dir, const ShaderInfo &si, int recursionDepth)
{
    unsigned int lightCount = mainScene->pointLights.size();

    Vector3 pixelColor = CalculateAmbientShader(si.shadingObject->material->ambient, mainScene->ambientLight);

    for(unsigned int lightIndex = 0; lightIndex < lightCount; lightIndex++)
    {
        const PointLight *currentPointLight = &mainScene->pointLights[lightIndex];

        if(si.shadingObject->material->mirror != Vector3(0))
        {
            pixelColor += CalculateMirrorReflectance(e, dir, si, recursionDepth);
        }

        if(si.shadingObject->material->transparency != Vector3(0))
        {
            pixelColor += CalculateTransparency(e, dir, si, recursionDepth);
        }

        // If the intersection point is in a shadow area, then don't make further calculations
        if (ShadowCheck(si.intersectionPoint, currentPointLight->position))
        {
            continue;
        }

        pixelColor += CalculateDiffuseShader(si.shadingObject->material->diffuse, currentPointLight->intensity, currentPointLight->position, si.intersectionPoint, si.shapeNormal);
        pixelColor += CalculateBlinnPhongShader(si.shadingObject->material->specular, si.shadingObject->material->phongExponent, currentPointLight->intensity, currentPointLight->position, si.ray.e, si.intersectionPoint, si.shapeNormal);
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

// e and dir are temporary fixes
Vector3 Renderer::CalculateMirrorReflectance(const Vector3 &e, const Vector3 &dir, const ShaderInfo &si, unsigned int recursionDepth)
{
    Vector3 wo = e - si.intersectionPoint;
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

            ShaderInfo newShaderInfo(Ray(o, wr), closestObject, newIntersectionPoint, closestN);

            return si.shadingObject->material->mirror * CalculateShader(o, wr, newShaderInfo, ++recursionDepth);
        }
    }

    return Vector3(0.f);
}

// e and dir are temporary fixes
Vector3 Renderer::CalculateTransparency(const Vector3 &e, const Vector3 &dir, const ShaderInfo& shaderInfo, unsigned int recursionDepth)
{
    if(recursionDepth >= mainScene->maxRecursionDepth)
    {
        return Vector3(0);
    }

    Vector3 refractionColor = Vector3(0);
    Vector3 reflectionColor = Vector3(0);

    
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