/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Renderer.h"

#include <algorithm>
#include <cstring>
#include <mutex>
#include <thread>

#include "BRDF.h"
#include "DirectionalLight.h"
#include "Scene.h"
#include "IOManager.h"
#include "LightMesh.h"
#include "LightSphere.h"
#include "Material.h"
#include "Mesh.h"
#include "ObjectBase.h"
#include "RandomGenerator.h"
#include "Texture.h"

std::mutex mut;

#define MAX_THREAD_COUNT 8

#define GAUSSIAN_VALUE(x, y) ((1 / TWO_PI) * (pow(NATURAL_LOGARITHM, -((x * x + y * y) * 0.5f))))
#define SCHLICKS_APPROXIMATION(cosTetha, R0) (R0 + (1 - R0) * pow(1 - cosTetha, 5))

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
        float *image = new float[colorSize];
        
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

        // SRGB Gamma Correction
        // if(currentCamera->gammaCorrection == GAMMA_CORRECTION::SRGB)
        // {          
        //     float maxColorValue = 255.f;
  
        //     for(size_t colorIndex = 0; colorIndex < colorSize; colorIndex++)
        //     {
        //         if(image[colorIndex] > maxColorValue)
        //         {
        //             maxColorValue = image[colorIndex];
        //         }
        //     }

        //     float oneOver2Point4 = 1 / 2.4f;

        //     for(size_t colorIndex = 0; colorIndex < colorSize; colorIndex++)
        //     {
        //         float normalizedValue = image[colorIndex] / maxColorValue;
        //         if(normalizedValue <= 0.031308f)
        //         {
        //             normalizedValue *= 12.92f;
        //         }
        //         else
        //         {
        //             normalizedValue = 1.055f * pow(normalizedValue, oneOver2Point4) - 0.055f;
        //         }

        //         image[colorIndex] = normalizedValue * maxColorValue;
        //     }
        // }

        if(currentCamera->TMO != TONE_MAPPING_TYPE::NONE)
        {
            float whiteLuminance = 0.f;
            double totalLogLuminance = 0.f;

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
            
            float *toneMappingImage = new float[colorSize];

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
            
            std::string pngImageName = currentCamera->imageName.substr(0, currentCamera->imageName.length() - 4);
            pngImageName += ".png";

            IOManager::WritePng(pngImageName.c_str(), imageWidth, imageHeight, toneMappingImage);
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

void Renderer::ThreadFunction(Camera *currentCamera, int startX, int startY, int width, int height, float *colorBuffer)
{
    if(height == 0) return;

    const RendererInfo ri(currentCamera);
    
    int pixelIndex = 3 * (startY * width + startX);

    unsigned int endX = startX + width;
    unsigned int endY = startY + height;

    for(unsigned int y = startY; y < endY; y++)
    {
        for(unsigned int x = startX; x < endX; x++)
        {
            Colorf pixelColor = RenderPixel(x + 0.5f, y + 0.5f, ri);

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
                        float randomU = RandomGenerator::GetRandomFloat();
                        float randomV = RandomGenerator::GetRandomFloat();

                        float gaussianValue = GAUSSIAN_VALUE((pSample + randomU) / pAmount, (qSample + randomV) / qAmount);

                        pixelColor += gaussianValue * RenderPixel(x + ((pSample + randomU) / pAmount) , y + ((qSample + randomV) / qAmount), ri);
                        divider += gaussianValue;
                    }
                }
            }
            pixelColor /= divider;

            colorBuffer[pixelIndex++] = pixelColor.r;
            colorBuffer[pixelIndex++] = pixelColor.g;
            colorBuffer[pixelIndex++] = pixelColor.b;
        }
    }
}

Colorf Renderer::RenderPixel(float x, float y, const RendererInfo &ri)
{
    Vector3 eye(ri.e);

    if(ri.camera->dopEnabled)
    {
        float randomX = RandomGenerator::GetRandomFloat();
        float randomY = RandomGenerator::GetRandomFloat();

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

    Colorf pixelColor = Colorf(0.f, 0.f, 0.f);

    Ray ray(eye, d);

    if(mainScene->SingleRayTrace(ray, closestT, closestN, beta, gamma, &closestObject))
    {
        pixelColor = Colorf(CalculateShader(ShaderInfo(ray, closestObject, eye + d * closestT, closestN, beta, gamma)));
    }
    else
    {
        pixelColor = mainScene->bgColor;
    }

    return pixelColor;
}

Vector3 Renderer::CalculateShader(const ShaderInfo &shaderInfo, int recursionDepth)
{
    if(recursionDepth >= mainScene->maxRecursionDepth)
    {
        return Vector3::ZeroVector;
    }

    if(const LightMesh* lightMesh = dynamic_cast<const LightMesh *>(shaderInfo.shadingObject->parentObject))
    {
        return lightMesh->intensity;
    }

    if(const LightSphere* lightSphere = dynamic_cast<const LightSphere *>(shaderInfo.shadingObject))
    {
        return lightSphere->intensity;
    }

    Vector3 textureColor = Vector3::ZeroVector;
    
    if(shaderInfo.shadingObject->texture)
    {
        textureColor = shaderInfo.shadingObject->GetTextureColorAt(shaderInfo.intersectionPoint, shaderInfo.beta, shaderInfo.gamma);
        
        if(shaderInfo.shadingObject->texture->decalMode == DECAL_MODE::REPLACE_ALL)
        {
            return textureColor;
        }
    }

    Vector3 pixelColor = CalculateAmbientShader(shaderInfo.shadingObject->material->ambient, mainScene->ambientLight);
    
    for(Light *light : mainScene->lights)
    {
        Vector3 lightPosition = light->GetPosition();

        // If the intersection point is in a shadow area, then don't make further calculations
        if (light->ShadowCheck(lightPosition, shaderInfo.intersectionPoint))
        {
            continue;
        }
 
        if(shaderInfo.shadingObject->material->mirror != Vector3::ZeroVector)
        {
            pixelColor += CalculateMirrorReflection(shaderInfo, recursionDepth);
        }

        if(shaderInfo.shadingObject->material->transparency != Vector3::ZeroVector)
        {
            pixelColor += CalculateTransparency(shaderInfo, recursionDepth);
        }
        
        Vector3 lightIntensity = light->GetIntensityAtPosition(lightPosition, shaderInfo.intersectionPoint);
        Vector3 wi = -light->GetDirection(shaderInfo.intersectionPoint);

/*         
        if(mainScene->integrator == INTEGRATOR::PATH_TRACER)
        {
            Vector3 directLightContribution = lightIntensity;

            Vector3 indirectLightContribution = Vector3::ZeroVector;
            for(unsigned int bounceIndex = 0; bounceIndex < mainScene->pathTracingBounceCount; bounceIndex++)
            {
                float bounceT;
                Vector3 bounceN;
                float bounceBeta, bounceGamma;
                const ObjectBase *bounceIntersectingObject;

                Vector3 randomRayDirection = Ray::GetRandomDirection(shaderInfo.shapeNormal);

                Ray bounceRay(shaderInfo.intersectionPoint + randomRayDirection * EPSILON, randomRayDirection);
                if(mainScene->SingleRayTrace(bounceRay, bounceT, bounceN, bounceBeta, bounceGamma, &bounceIntersectingObject))
                {
                    indirectLightContribution += bounceIntersectingObject->material->diffuse * CalculateShader(ShaderInfo(bounceRay, bounceIntersectingObject, bounceRay.e + bounceRay.dir * bounceT, bounceN, bounceBeta, bounceGamma), ++recursionDepth);
                }
            }
            indirectLightContribution /= mainScene->pathTracingBounceCount;

            lightIntensity = directLightContribution + indirectLightContribution;
        } 
*/

        Vector3 diffuseColor;

        if(shaderInfo.shadingObject->texture)
        {
            textureColor /= shaderInfo.shadingObject->texture->normalizer;
            textureColor.Clamp(0.f, 1.f);

            if(shaderInfo.shadingObject->texture->decalMode == DECAL_MODE::REPLACE_KD)
            {
                diffuseColor = textureColor;
            }
            else
            {
                diffuseColor = (shaderInfo.shadingObject->material->diffuse + textureColor) * 0.5f;
            }
        }
        else
        {
            diffuseColor = shaderInfo.shadingObject->material->diffuse;
        }

        if(shaderInfo.shadingObject->material->brdf)
        {
            Vector3 wo = shaderInfo.ray.e - shaderInfo.intersectionPoint;
            wo.Normalize();
            
            pixelColor += shaderInfo.shadingObject->material->brdf->GetBRDF(diffuseColor, shaderInfo.shadingObject->material->specular, shaderInfo.shapeNormal, wo, wi) * lightIntensity;
        }
        else
        {
            pixelColor += CalculateDiffuseShader(shaderInfo, diffuseColor, wi, lightIntensity);
            pixelColor += CalculateSpecularShader(shaderInfo, wi, lightIntensity);
        }

        if(mainScene->integrator == INTEGRATOR::PATH_TRACER)
        {
            Vector3 indirectLightContribution = Vector3::ZeroVector;
            for(unsigned int bounceIndex = 0; bounceIndex < mainScene->pathTracingBounceCount; bounceIndex++)
            {
                float bounceT;
                Vector3 bounceN;
                float bounceBeta, bounceGamma;
                const ObjectBase *bounceIntersectingObject;

                Vector3 randomRayDirection = Ray::GetRandomDirection(shaderInfo.shapeNormal);

                Ray bounceRay(shaderInfo.intersectionPoint + randomRayDirection * EPSILON, randomRayDirection);
                if(mainScene->SingleRayTrace(bounceRay, bounceT, bounceN, bounceBeta, bounceGamma, &bounceIntersectingObject))
                {
                    indirectLightContribution += CalculateShader(ShaderInfo(bounceRay, bounceIntersectingObject, bounceRay.e + bounceRay.dir * bounceT, bounceN, bounceBeta, bounceGamma), ++recursionDepth)/*  / TWO_PI */;
                }
            }
            indirectLightContribution /= mainScene->pathTracingBounceCount;

            pixelColor += indirectLightContribution;
        }
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

// Blinn-Phong Shader
Vector3 Renderer::CalculateSpecularShader(const ShaderInfo& shaderInfo, const Vector3 &wi, const Vector3 &lightIntensity)
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
        wr = Ray::GetRandomDirection(wr);
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