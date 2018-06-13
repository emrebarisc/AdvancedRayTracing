/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __DIRECTIONALLIGHT_H__
#define __DIRECTIONALLIGHT_H__

#include "Light.h"

class DirectionalLight : public Light
{
public:
    DirectionalLight() : Light()
    {

    }

    ~DirectionalLight() override
    {
        
    }

    Vector3 GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const override;

    Vector3 GetDirection(const Vector3 &referencePosition) const override
    {
        return direction;
    }

    bool ShadowCheck(const Vector3& lightPosition, const Vector3& positionAt) const override;

    Vector3 direction;
    Vector3 radiance;
private:
};

#endif