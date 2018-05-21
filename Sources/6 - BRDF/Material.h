/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "Math.h"

/*
    Material class containing ambient, diffuse, phong specular, and mirror properties
*/
class Material
{
public:
    Material()
    {

    }

    ~Material()
    {

    }

    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;
    Vector3 mirror;
    Vector3 transparency;
    float roughness;
    float refractionIndex;
    float phongExponent;

    bool degamma;

private:

};

#endif