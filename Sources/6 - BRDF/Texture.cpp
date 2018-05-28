/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Texture.h"

#include "IOManager.h"
#include "Math.h"
#include "Scene.h"

bool Texture::LoadTextureImage()
{
    IOManager::GetJpgSize(imagePath.c_str(), width, height);

    if(textureImage == nullptr)
    {
        InitTextureImageArray();
    }

    return IOManager::ReadJpg(imagePath.c_str(), width, height, textureImage);
}

void Texture::GetIntersectingUV(float &u, float &v)
{

}

Vector3 Texture::Fetch(int i, int j, int w /* = 0.f */) const
{
    int index = 3 * (j * width + i);

    Vector3 color;
    color.x = textureImage[index];
    color.y = textureImage[index + 1];
    color.z = textureImage[index + 2];

    return color;
}

Vector3 Texture::GetInterpolatedUV(float u, float v, float w /* = 0.f */) const
{
    if(interpolationMethod == INTERPOLATION::NEAREST)
    {
        int i = round(u * width);
        int j = round(v * height);
        return Fetch(i, j);
    }

    // Else if interpolationMethod == INTERPOLATION::BILINEAR
    float i = u * width;
    float j = v * height;

    int p = floor(i);
    int q = floor(j);
    float dx = i - p;
    float dy = j - q;
                   
    return Fetch(p    , q    ) * (1 - dx) * (1 - dy) +
           Fetch(p + 1, q    ) * (dx    ) * (1 - dy) +
           Fetch(p    , q + 1) * (1 - dx) * (dy    ) +
           Fetch(p + 1, q + 1) * (dx    ) * (dy    );
}

Vector3 Texture::GetBumpNormal(const Vector3& n, float u, float v, const Vector3 &pU, const Vector3 &pV)
{
    float referenceU = u < 1 - INTERSECTION_TEST_EPSILON ? u + INTERSECTION_TEST_EPSILON : u - INTERSECTION_TEST_EPSILON;
    float referenceV = v < 1 - INTERSECTION_TEST_EPSILON ? v + INTERSECTION_TEST_EPSILON : v - INTERSECTION_TEST_EPSILON;

    Vector3 textureColor = GetInterpolatedUV(u, v);
    Vector3 referenceTextureColor = GetInterpolatedUV(referenceU, v);
    float averageColorDU = (textureColor.x + textureColor.y + textureColor.z) /* / 3.f * bumpmapMultiplier */;
    float averageReferenceDU = (referenceTextureColor.x + referenceTextureColor.y + referenceTextureColor.z)/*  / 3.f * bumpmapMultiplier */;
    float du = (averageReferenceDU - averageColorDU) / 6.f * bumpmapMultiplier;

    referenceTextureColor = GetInterpolatedUV(u, referenceV);
    float averageColorDV = (textureColor.x + textureColor.y + textureColor.z) /* / 3.f * bumpmapMultiplier */;
    float averageReferenceDV = (referenceTextureColor.x + referenceTextureColor.y + referenceTextureColor.z)/* / 3.f * bumpmapMultiplier */;
    float dv = (averageReferenceDV - averageColorDV) / 6.f * bumpmapMultiplier;

    Vector3 nPrime = Vector3::Cross(pV + dv * n, pU + du * n);
    //Vector3 nPrime = n + du * Vector3::Cross(pV, n).GetNormalized() + dv * Vector3::Cross(n, pU).GetNormalized();

    return nPrime;
}