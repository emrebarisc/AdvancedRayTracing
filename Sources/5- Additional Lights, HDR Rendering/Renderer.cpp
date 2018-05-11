/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Renderer.h"

#include <algorithm>
#include <cstring>
#include <mutex>
#include <random>
#include <thread>

#include "DirectionalLight.h"
#include "Scene.h"
#include "IOManager.h"
#include "Material.h"
#include "Mesh.h"
#include "ObjectBase.h"
#include "Texture.h"

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

        unsigned int imageSize = imageWidth * imageHeight;
        unsigned int colorSize = imageSize * 3;
        //unsigned char *image = new unsigned char[colorSize];
        int *image = new int[colorSize];
        
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

        if(currentCamera->TMO != TONE_MAPPING_TYPE::NONE)
        {
            float whiteLuminance = 0.f;
            float totalLogLuminance = 0.f;

            std::vector<float> luminanceValues;

            for(unsigned int colorIndex = 0; colorIndex < colorSize; colorIndex += 3)
            {
                float luminance = 0.27f * image[colorIndex] + 0.67f * image[colorIndex + 1] + 0.06f * image[colorIndex + 2];
                //float luminance = 0.2126f * image[colorIndex] + 0.7152f * image[colorIndex + 1] + 0.0722f * image[colorIndex + 2];

                luminanceValues.push_back(luminance);
                totalLogLuminance += log(luminance + EPSILON);
            }

            std::make_heap(luminanceValues.begin(), luminanceValues.end());
            std::sort_heap(luminanceValues.begin(), luminanceValues.end());

            float whiteLuminanceIndex = (100.f - currentCamera->TMOOptions.y) / 100.f;
            whiteLuminance = luminanceValues[round(luminanceValues.size() * whiteLuminanceIndex)] / luminanceValues[luminanceValues.size() - 1];

            totalLogLuminance /= imageSize;
            float logAverageLuminance = exp(totalLogLuminance);
            
            int *toneMappingImage = new int[colorSize];

            for(unsigned int colorIndex = 0; colorIndex < colorSize; colorIndex += 3)
            {
                float luminance = 0.27f * image[colorIndex] + 0.67f * image[colorIndex + 1] + 0.06f * image[colorIndex + 2];
                //float luminance = 0.2126f * image[colorIndex] + 0.7152f * image[colorIndex + 1] + 0.0722f * image[colorIndex + 2];

                float scaledLuminance = luminance * currentCamera->TMOOptions.x / logAverageLuminance;
                //float displayLuminance = scaledLuminance / (1 + scaledLuminance);
                float displayLuminance = (scaledLuminance * (1 + (scaledLuminance / (whiteLuminance * whiteLuminance)))) / (1 + scaledLuminance);
                float finalLuminance = displayLuminance;

                float displayR = mathClamp(pow(image[colorIndex] / luminance, currentCamera->saturation) * finalLuminance, 0, 1);
                float displayG = mathClamp(pow(image[colorIndex + 1] / luminance, currentCamera->saturation) * finalLuminance, 0, 1);
                float displayB = mathClamp(pow(image[colorIndex + 2] / luminance, currentCamera->saturation) * finalLuminance, 0, 1);
                
                toneMappingImage[colorIndex    ] = pow(displayR, 0.45f) * 255;
                toneMappingImage[colorIndex + 1] = pow(displayG, 0.45f) * 255;
                toneMappingImage[colorIndex + 2] = pow(displayB, 0.45f) * 255;
            }
            IOManager::WritePng("HDR_Test.png", imageWidth, imageHeight, toneMappingImage);
        }
        
        std::string extension;
        unsigned int imageNameLength = currentCamera->imageName.length();
        
        for(int nameIndex = imageNameLength - 1; nameIndex >= 0; nameIndex--)
        {
            if(currentCamera->imageName.c_str()[nameIndex] != '.')
            {
                extension.insert(0, 1, currentCamera->imageName.c_str()[nameIndex]);
            }
            else
            {
                break;
            }
        }

        if(extension == "png") IOManager::WritePng(currentCamera->imageName.c_str(), imageWidth, imageHeight, image);
        else if(extension == "exr") IOManager::WriteExr(currentCamera->imageName.c_str(), imageWidth, imageHeight, image);
        else if(extension == "ppm") true;//IOManager::WritePpm(currentCamera->imageName.c_str(), imageWidth, imageHeight, image);
        else std::cerr << "Output extension is unknown! Extension is: " << extension << std::endl; 
    }
}

void Renderer::ThreadFunction(Camera *currentCamera, int startX, int startY, int width, int height, int *colorBuffer)
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
            //pixelColor.ClampColor(0, 255);

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
    d.Normalize();

    float closestT = -1;
    float beta, gamma;
    Vector3 closestN = Vector3::ZeroVector;
    const ObjectBase *closestObject = nullptr;

    Colori pixelColor = Vector3::ZeroVector;

    Ray ray(eye, d);

    if(mainScene->SingleRayTrace(ray, closestT, closestN, beta, gamma, &closestObject))
    {
        pixelColor = Colori(CalculateShader(ShaderInfo(ray, closestObject, eye + d * closestT, closestN, beta, gamma)));
    }
    else
    {
        pixelColor = mainScene->bgColor;
    }

    return pixelColor;
}

Vector3 Renderer::CalculateShader(const ShaderInfo &si, int recursionDepth)
{
    unsigned int lightCount = mainScene->lights.size();

    Vector3 pixelColor = CalculateAmbientShader(si.shadingObject->material->ambient, mainScene->ambientLight);

    for(unsigned int lightIndex = 0; lightIndex < lightCount; lightIndex++)
    {
        Vector3 textureColor = Vector3::ZeroVector;

        if(si.shadingObject->texture)
        {
            textureColor = si.shadingObject->GetTextureColorAt(si.intersectionPoint, si.beta, si.gamma);
            
            if(si.shadingObject->texture->decalMode == DECAL_MODE::REPLACE_ALL)
            {
                return textureColor;
            }
        }
        
        const Light *light = mainScene->lights[lightIndex];

        Vector3 lightPosition = light->GetPosition();

        // If the intersection point is in a shadow area, then don't make further calculations
        if (light->ShadowCheck(lightPosition, si.intersectionPoint))
        {
            continue;
        }
 
        if(si.shadingObject->material->mirror != Vector3::ZeroVector)
        {
            pixelColor += CalculateMirrorReflection(si, recursionDepth);
        }

        if(si.shadingObject->material->transparency != Vector3::ZeroVector)
        {
            pixelColor += CalculateTransparency(si, recursionDepth);
        }
        
        Vector3 lightIntensity = light->GetIntensityAtPosition(lightPosition, si.intersectionPoint);

        Vector3 wi;
        const DirectionalLight *directionalLight;
        if(directionalLight = dynamic_cast<const DirectionalLight *>(light))
        {
            wi = -directionalLight->direction;
        }
        else
        {
            wi = lightPosition - si.intersectionPoint;
        }
        wi.Normalize();

        if(si.shadingObject->texture)
        {
            textureColor /= si.shadingObject->texture->normalizer;
            textureColor.Clamp(0.f, 1.f);

            if(si.shadingObject->texture->decalMode == DECAL_MODE::REPLACE_KD)
            {
                pixelColor += CalculateDiffuseShader(si, textureColor, wi, lightIntensity);
            }
            else
            {
                pixelColor += CalculateDiffuseShader(si, (si.shadingObject->material->diffuse + textureColor) * 0.5f, wi, lightIntensity);
            }
        }
        else
        {
            pixelColor += CalculateDiffuseShader(si, si.shadingObject->material->diffuse, wi, lightIntensity);
        }
        
        pixelColor += CalculateBlinnPhongShader(si, wi, lightIntensity);
    }

    return pixelColor;
}

Vector3 Renderer::CalculateAmbientShader(const Vector3& ambientReflectance, const Vector3& intensity)
{
  return ambientReflectance * intensity;
}

Vector3 Renderer::CalculateDiffuseShader(const ShaderInfo& shaderInfo, const Vector3 &diffuse, const Vector3 &wi, const Vector3 &lightIntensity)
{
    // float cosTethaPrime = mathMax(0, Vector3::Dot(wi, shaderInfo.shapeNormal));

    return diffuse * mathMax(0, Vector3::Dot(wi, shaderInfo.shapeNormal)) * lightIntensity;
}

Vector3 Renderer::CalculateBlinnPhongShader(const ShaderInfo& shaderInfo, const Vector3 &wi, const Vector3 &lightIntensity)
{
    Vector3 wo = shaderInfo.ray.e - shaderInfo.intersectionPoint;
    wo.Normalize();
    Vector3 h = (wi + wo) / (wi + wo).Length();

    float cosAlphaPrime = mathMax(0, Vector3::Dot(shaderInfo.shapeNormal, h));

    return shaderInfo.shadingObject->material->specular * std::pow(cosAlphaPrime, shaderInfo.shadingObject->material->phongExponent) * lightIntensity;
}

Vector3 Renderer::CalculateReflection(const ShaderInfo &shaderInfo, unsigned int recursionDepth)
{
    Vector3 wo = shaderInfo.ray.e - shaderInfo.intersectionPoint;
    wo.Normalize();
    Vector3 wr = -wo + 2 * shaderInfo.shapeNormal * Vector3::Dot(shaderInfo.shapeNormal, wo);
    wr.Normalize();

    float closestT;
    float beta, gamma;
    Vector3 closestN;
    const ObjectBase* closestObject;
    
    if(shaderInfo.shadingObject->material->roughness != 0.f)
    {
        std::uniform_real_distribution<float> uniformDistribution(0.0, 1.0);
        float randomU = uniformDistribution(randomGenerator);
        float randomV = uniformDistribution(randomGenerator);

        Vector3 rPrime = wr.GetOrthonormalBasis();
        rPrime.Normalize();
        Vector3 u = Vector3::Cross(rPrime, wr);
        u.Normalize();
        Vector3 v = Vector3::Cross(u, wr);
        v.Normalize();
        wr += shaderInfo.shadingObject->material->roughness * (u * randomU + v * randomV);
        wr.Normalize();
    }

    Vector3 o = shaderInfo.intersectionPoint + (wr * INTERSECTION_TEST_EPSILON);

    Ray ray(o, wr);
    if(mainScene->SingleRayTrace(ray, closestT, closestN, beta, gamma, &closestObject))
    {
        return CalculateShader(ShaderInfo(ray, closestObject, o + wr * closestT, closestN, beta, gamma), ++recursionDepth);
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
    float beta, gamma;
    Vector3 hitN;
    const ObjectBase *hitObject;
    
    Vector3 o = shaderInfo.intersectionPoint - normal * INTERSECTION_TEST_EPSILON;

    Ray ray(o, t);
    if(mainScene->SingleRayTrace(ray, hitT, hitN, beta, gamma, &hitObject))
    {
        Vector3 nextIntersectionPoint = shaderInfo.intersectionPoint + hitT * t;
        ShaderInfo reflectedShaderInfo(Ray(o, t), hitObject, nextIntersectionPoint, hitN, beta, gamma);

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