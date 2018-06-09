/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __AREALIGHT_H__
#define __AREALIGHT_H__

#include "Light.h"

class AreaLight : public Light
{
public:
    AreaLight()
    {
        
    }

    ~AreaLight() override
    {
        
    }

    Vector3 GetPosition() const override;
    virtual Vector3 GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const override;

    Vector3 edgeVectorU;
    Vector3 edgeVectorV;
    Vector3 lightNormal;

private:
};

#endif