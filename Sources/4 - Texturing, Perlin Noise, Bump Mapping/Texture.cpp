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
                    
    return color;
}