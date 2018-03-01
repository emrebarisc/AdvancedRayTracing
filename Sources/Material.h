#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "Color.h"

class Material
{
public:
    Material()
    {

    }

    ~Material()
    {

    }

    Colorf ambient;
    Colorf diffuse;
    Colorf specular;
    Colorf mirror;
    float phongExponent;

private:

};

#endif