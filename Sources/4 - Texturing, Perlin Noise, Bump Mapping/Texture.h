/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <string>
#include "Material.h"

struct Scene;

enum INTERPOLATION : uint8_t
{
  NEAREST = 0,
  BILINEAR
};

enum DECAL_MODE : uint8_t
{
  REPLACE_KD = 0,
  BLEND_KD,
  REPLACE_ALL
};

enum APPEARANCE : uint8_t
{
  CLAMP = 0,
  REPEAT,
  VEIN,
  PETCH
};

class Texture
{
public:
  Texture() : width(0), height(0)
  {
    textureImage = nullptr;
  }

  Texture(int w, int h) : width(w), height(h)
  {
    textureImage = new unsigned char[width * height * 3];
  }

  Texture(int w, int h, std::string path) : imagePath(path), width(w), height(h)
  {
    textureImage = new unsigned char[width * height * 3];
  }
  
  void InitTextureImageArray()
  {
    if(height == 0 || width == 0)
    {
      std::cerr << "Image height or width is 0." << std::endl;
      exit(EXIT_FAILURE);
    }
    
    textureImage = new unsigned char[width * height * 3];
  }

  bool LoadTextureImage();

  Vector3 GetTextureColorAt(int triA, int triB, int triC, float beta, float gamma) const;
  Vector3 Fetch(int u, int v, int w = 0.f) const;
  virtual Vector3 GetInterpolatedUV(float u, float v, float w = 0.f) const;

  std::string imagePath;
  INTERPOLATION interpolationMethod;
  DECAL_MODE decalMode;
  APPEARANCE appearance;

  unsigned char* textureImage;

  int width, height;
  int normalizer;
private:

};

#endif