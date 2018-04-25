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

Vector3 Texture::GetTextureColorAt(int triA, int triB, int triC, float beta, float gamma) const
{
    Vector2i uvCoordA = mainScene->textureCoordinates[triA - 1];
    Vector2i uvCoordB = mainScene->textureCoordinates[triB - 1];
    Vector2i uvCoordC = mainScene->textureCoordinates[triC - 1];

    Vector2 uv;
    uv.x = uvCoordA.x + beta * (uvCoordB.x - uvCoordA.x) + gamma * (uvCoordC.x - uvCoordA.x);
    uv.y = uvCoordA.y + beta * (uvCoordB.y - uvCoordA.y) + gamma * (uvCoordC.y - uvCoordA.y);

    if(appearance == APPEARANCE::REPEAT)
    {
        uv.x -= floor(uv.x);
        uv.y -= floor(uv.y);
    }

    return GetInterpolatedUV(uv.x, uv.y);
}

Vector3 Texture::Fetch(int i, int j) const
{
    int index = 3 * (j * width + i);

    Vector3 color;
    color.x = textureImage[index];
    color.y = textureImage[index + 1];
    color.z = textureImage[index + 2];

    return color;
}

Vector3 Texture::GetInterpolatedUV(float u, float v) const
{
    if(interpolationMethod == INTERPOLATION::NEAREST)
    {
        int i = round(u * width);
        int j = round(v * height);
        return Fetch(i, j);
    }

    // Else if interpolationMethod == INTERPOLATION::BILINEAR
    int i = u * width;
    int j = v * height;

    int p = floor(i);
    int q = floor(j);
    float dx = i - p;
    float dy = j - q;

    Vector3 color = Fetch(p    , q    ) * (1 - dx) * (1 - dy) +
                    Fetch(p + 1, q    ) * (dx    ) * (1 - dy) +
                    Fetch(p    , q + 1) * (1 - dx) * (dy    ) +
                    Fetch(p + 1, q + 1) * (dx    ) * (dy    );

    color.Clamp(0, normalizer);
    return color;
}