/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Renderer.h"

#include <thread>
#include <mutex>

#include "Scene.h"
#include "OutputManager.h"
#include "Material.h"
#include "ObjectBase.h"

#define MAX_THREAD_COUNT 8

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

    Vector3 m, q;
    Vector3 e, w, u, v;
    float l, r, b, t;
    float distance;

    e = currentCamera->position;
    w = currentCamera->gaze;
    v = currentCamera->up;
    u = currentCamera->right;

    l = currentCamera->nearPlane.x;
    r = currentCamera->nearPlane.y;
    b = currentCamera->nearPlane.z;
    t = currentCamera->nearPlane.w;

    distance = currentCamera->nearDistance;

    m = e + (w * distance);
    q = m + u * l + v * t;

    float su, sv;
    Vector3 s, ray, d;

    int pixelIndex = 3 * (startY * width + startX);

    unsigned int endX = startX + width;
    unsigned int endY = startY + height;

    for(int y = startY; y < endY; y++)
    {
        sv = (t - b) * (y + 0.5f) / imageHeight;

        for(int x = startX; x < endX; x++)
        {
            su = (r - l) * (x + 0.5f) / imageWidth;
            s = q + (u * su) - (v * sv);
            d = s - e;
            ray = e + d;

            float closestT = -1;
            Vector3 closestN = Vector3::ZeroVector();
            ObjectBase *closestObject = nullptr;

            if(mainScene->SingleRayTrace(e, d, closestT, closestN, &closestObject))
            {
                Vector3 intersectionPoint = e + d * closestT;

                Colori pixelColor = Colori(CalculateShader(closestObject->material, currentCamera->position, intersectionPoint, closestN));
                pixelColor.ClampColor(0, 255); 

                colorBuffer[pixelIndex++] = pixelColor.r;
                colorBuffer[pixelIndex++] = pixelColor.g;
                colorBuffer[pixelIndex++] = pixelColor.b;
            }
            else
            {
                colorBuffer[pixelIndex++] = mainScene->bgColor.r;
                colorBuffer[pixelIndex++] = mainScene->bgColor.g;
                colorBuffer[pixelIndex++] = mainScene->bgColor.b;
            }
        }
    }
}

Vector3 Renderer::CalculateShader(const Material* material, const Vector3& camPos, const Vector3& intersectionPoint, const Vector3& shapeNormal, int recursionDepth)
{
    unsigned int lightCount = mainScene->pointLights.size();

    Vector3 pixelColor = CalculateAmbientShader(material->ambient, mainScene->ambientLight);

    for(unsigned int lightIndex = 0; lightIndex < lightCount; lightIndex++)
    {
        const PointLight *currentPointLight = &mainScene->pointLights[lightIndex];

        if(material->mirror != Vector3::ZeroVector())
        {
            Vector3 wo = camPos - intersectionPoint;
            Vector3::Normalize(wo);
            Vector3 wr = -wo + 2 * shapeNormal * Vector3::Dot(shapeNormal, wo);
            Vector3::Normalize(wr);

            Vector3 o = intersectionPoint + (wr * EPSILON);

            float closestT;
            Vector3 closestN;
            ObjectBase* closestObject;
            if(mainScene->SingleRayTrace(o, wr, closestT, closestN, &closestObject))
            {
                if(recursionDepth < mainScene->maxRecursionDepth)
                {
                    Vector3 newIntersectionPoint = o + wr * closestT;
                    pixelColor += material->mirror * CalculateShader(closestObject->material, intersectionPoint, newIntersectionPoint, closestN, ++recursionDepth);
                }
            }
        }

        // If the intersection point is in a shadow area, then don't make further calculations
        if (ShadowCheck(intersectionPoint, currentPointLight->position))
        {
            continue;
        }

        pixelColor += CalculateDiffuseShader(material->diffuse, currentPointLight->intensity, currentPointLight->position, intersectionPoint, shapeNormal);
        pixelColor += CalculateBlinnPhongShader(material->specular, material->phongExponent, currentPointLight->intensity, currentPointLight->position, camPos, intersectionPoint, shapeNormal);
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