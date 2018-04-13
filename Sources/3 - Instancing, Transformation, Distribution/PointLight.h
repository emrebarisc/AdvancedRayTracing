/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __POINTLIGHT_H__
#define __POINTLIGHT_H__

#include "Light.h"

class PointLight : public Light
{
public:
    PointLight()
    {

    }

    ~PointLight()
    {
        
    }
    
    Vector3 GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const override;
};

#endif