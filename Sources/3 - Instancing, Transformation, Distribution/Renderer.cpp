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
#define SCHLICKS_APPROXIMATION(cosTetha, R0) (R0 + (1 - R0) * pow(1 - cosTetha, 5))

std::random_device randomDevice;  //Will be used to obtain a seed for the random number engine
std::mt19937 randomGenerator(randomDevice()); //Standard mersenne_twister_engine seeded with randomDevice()

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

    const RendererInfo ri(currentCamera);
    
    int pixelIndex = 3 * (startY * width + startX);

    unsigned int endX = startX + width;
    unsigned int endY = startY + height;

    std::uniform_real_distribution<float> uniformDistribution(0.0, 1.0);

    for(unsigned int y = startY; y < endY; y++)
    {
        for(unsigned int x = startX; x < endX; x++)
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
    Vector3 eye(ri.e);

    if(ri.camera->dopEnabled)
    {
        std::uniform_real_distribution<float> uniformDistribution(-ri.camera->apartureSize * 0.5f, ri.camera->apartureSize * 0.5f);

        float randomX = uniformDistribution(randomGenerator);
        float randomY = uniformDistribution(randomGenerator);

        eye += ri.camera->right * randomX;
        eye += ri.camera->up * randomY;
    }

    float su = (ri.r - ri.l) * x / imageWidth;
    float sv = (ri.t - ri.b) * y / imageHeight;

    Vector3 s = ri.q + (ri.u * su) - (ri.v * sv);
    Vector3 d = s - eye;
    Vector3::Normalize(d);

    float closestT = -1;
    Vector3 closestN = Vector3::ZeroVector;
    ObjectBase *closestObject = nullptr;

    Colori pixelColor = Vector3::ZeroVector;

    Ray ray(eye, d);

    if(mainScene->SingleRayTrace(ray, closestT, closestN, &closestObject))
    {
        Vector3 intersectionPoint = eye + d * closestT;

        ShaderInfo si(ray, closestObject, intersectionPoint, closestN);

        pixelColor = Colori(CalculateShader(si));
    }
    else
    {
        pixelColor = mainScene->bgColor;
    }

    return pixelColor;
}

Vector3 Renderer::CalculateShader(const ShaderInfo &si, int recursionDepth)
{
    unsigned int lightCount = mainScene->pointLights.size();

    Vector3 pixelColor = CalculateAmbientShader(si.shadingObject->material->ambient, mainScene->ambientLight);

    for(unsigned int lightIndex = 0; lightIndex < lightCount; lightIndex++)
    {
        const PointLight *currentPointLight = &mainScene->pointLights[lightIndex];

        if(si.shadingObject->material->mirror != Vector3::ZeroVector)
        {
            pixelColor += CalculateMirrorReflection(si, recursionDepth);
        }

        if(si.shadingObject->material->transparency != Vector3::ZeroVector)
        {
            pixelColor += CalculateTransparency(si, recursionDepth);
        }

        // If the intersection point is in a shadow area, then don't make further calculations
        if (ShadowCheck(si.intersectionPoint, currentPointLight->position))
        {
            continue;
        }

        pixelColor += CalculateDiffuseShader(si, currentPointLight);
        pixelColor += CalculateBlinnPhongShader(si, currentPointLight);
    }

    return pixelColor;
}

Vector3 Renderer::CalculateAmbientShader(const Vector3& ambientReflectance, const Vector3& intensity)
{
  return ambientReflectance * intensity;
}

Vector3 Renderer::CalculateDiffuseShader(const ShaderInfo& shaderInfo, const Light* light)
{
    if(light == nullptr) return shaderInfo.shadingObject->material->diffuse;

    float distance = (shaderInfo.intersectionPoint - light->position).Length();

    Vector3 iOverRSquare = light->intensity / (distance * distance);
    Vector3 wi = light->position - shaderInfo.intersectionPoint;
    wi /= wi.Length();
    float cosTethaPrime = mathMax(0, Vector3::Dot(wi, shaderInfo.shapeNormal));

    return shaderInfo.shadingObject->material->diffuse * cosTethaPrime * iOverRSquare;
}

Vector3 Renderer::CalculateBlinnPhongShader(const ShaderInfo& shaderInfo, const Light* light)
{
    if(light == nullptr) return shaderInfo.shadingObject->material->specular;

    float distance = (shaderInfo.intersectionPoint - light->position).Length();

    Vector3 wi = light->position - shaderInfo.intersectionPoint;
    wi.Normalize();
    Vector3 wo = shaderInfo.ray.e - shaderInfo.intersectionPoint;
    wo.Normalize();
    Vector3 h = (wi + wo) / (wi + wo).Length();

    Vector3 incomingRadiance = light->intensity / (distance * distance);
    float cosAlphaPrime = mathMax(0, Vector3::Dot(shaderInfo.shapeNormal, h));

    return shaderInfo.shadingObject->material->specular * std::pow(cosAlphaPrime, shaderInfo.shadingObject->material->phongExponent) * incomingRadiance;
}

Vector3 Renderer::CalculateReflection(const ShaderInfo &shaderInfo, unsigned int recursionDepth)
{
    Vector3 wo = shaderInfo.ray.e - shaderInfo.intersectionPoint;
    wo.Normalize();
    Vector3::Normalize(wo);
    Vector3 wr = -wo + 2 * shaderInfo.shapeNormal * Vector3::Dot(shaderInfo.shapeNormal, wo);
    Vector3::Normalize(wr);

    Vector3 o = shaderInfo.intersectionPoint + (wr * MIRROR_EPSILON);

    float closestT;
    Vector3 closestN;
    ObjectBase* closestObject;

    Ray ray(o, wr);
    if(mainScene->SingleRayTrace(ray, closestT, closestN, &closestObject))
    {
        if(recursionDepth < mainScene->maxRecursionDepth)
        {
            Vector3 newIntersectionPoint = o + wr * closestT;

            ShaderInfo newShaderInfo(Ray(o, wr), closestObject, newIntersectionPoint, closestN);

            return CalculateShader(newShaderInfo, ++recursionDepth);
        }
    }

    return Vector3::ZeroVector;
}

Vector3 Renderer::CalculateRefraction(const ShaderInfo &shaderInfo, float &fresnel, unsigned int recursionDepth)
{
    Vector3 normal = shaderInfo.shapeNormal;
    float n1 = 1, n2 = 1;

    Vector3 attenuation = Vector3::ZeroVector;
    float cosTetha = Vector3::Dot(shaderInfo.ray.dir, normal);

    if(cosTetha > 0)
    {
        normal = -shaderInfo.shapeNormal;
        n1 = shaderInfo.shadingObject->material->refractionIndex;
        n2 = 1;
        cosTetha = Vector3::Dot(shaderInfo.ray.dir, normal);
    }
    else
    {
        n1 = 1;
        n2 = shaderInfo.shadingObject->material->refractionIndex;
        
        attenuation = Vector3(1.f);
    }

    float fractionDivision = n1 / n2;
    float delta = 1 - pow(fractionDivision, 2) * (1 - pow(cosTetha, 2));

    if(cosTetha > 0 && delta < 0)
    {
        return Vector3::ZeroVector;
    }
    
    float cosPhi = sqrt(delta);
    Vector3 t = (shaderInfo.ray.dir - normal * cosTetha) * fractionDivision - normal * cosPhi;

    if(cosTetha > 0)
    {
        cosTetha = Vector3::Dot(t, -normal);
    }
    else
    {
        cosTetha = Vector3::Dot(-shaderInfo.ray.dir, normal);
    }

    float R0 = pow(shaderInfo.shadingObject->material->refractionIndex - 1, 2) / pow(shaderInfo.shadingObject->material->refractionIndex + 1, 2);
    fresnel = R0 + (1 - R0) * pow(1 - mathMax(0.f, cosTetha), 5);

    /* if(cosTetha > 0)
    {
        attenuation = pow(0.99f, tLength);
    } */

    float hitT;
    Vector3 hitN;
    ObjectBase *hitObject;
    
    Vector3 o = shaderInfo.intersectionPoint - normal * EPSILON;

    Ray ray(o, t);
    if(mainScene->SingleRayTrace(ray, hitT, hitN, &hitObject))
    {
        Vector3 nextIntersectionPoint = shaderInfo.intersectionPoint + hitT * t;
        ShaderInfo reflectedShaderInfo(Ray(o, t), hitObject, nextIntersectionPoint, hitN);

        return /* attenuation *  */CalculateShader(reflectedShaderInfo, ++recursionDepth);
    }
    
    return Vector3::ZeroVector;
}

Vector3 Renderer::CalculateMirrorReflection(const ShaderInfo &shaderInfo, unsigned int recursionDepth)
{
    if(recursionDepth >= mainScene->maxRecursionDepth)
    {
        return Vector3::ZeroVector;
    }

    return shaderInfo.shadingObject->material->mirror * CalculateReflection(shaderInfo, recursionDepth);
}

Vector3 Renderer::CalculateTransparency(const ShaderInfo& shaderInfo, unsigned int recursionDepth)
{
    if(recursionDepth >= mainScene->maxRecursionDepth)
    {
        return Vector3::ZeroVector;
    }

    float fresnel = 0.f;
    Vector3 refractionColor = CalculateRefraction(shaderInfo, fresnel, recursionDepth);
    Vector3 reflectionColor = CalculateReflection(shaderInfo, recursionDepth);

    return shaderInfo.shadingObject->material->transparency * ( (1 - fresnel) * refractionColor + fresnel * reflectionColor);    
}

bool Renderer::ShadowCheck(const Vector3 &intersectionPoint, const Vector3 &lightPos)
{
    float distance = (lightPos - intersectionPoint).Length();
    Vector3 wi = (lightPos - intersectionPoint).GetNormalized();
    Vector3 o = intersectionPoint + wi * EPSILON;

    float closestT = 0;
    Vector3 closestN;
    ObjectBase *closestObject = nullptr;

    Ray ray(o, wi);
    mainScene->SingleRayTrace(ray, closestT, closestN, &closestObject, true);

    return closestT > 0 && closestT < distance;
}