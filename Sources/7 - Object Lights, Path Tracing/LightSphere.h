/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __LIGHTSPHERE_H__
#define __LIGHTSPHERE_H__

#include "Light.h"
#include "Sphere.h"

class LightSphere : public Light, public Sphere
{
public:
    LightSphere()
    {

    }

    ~LightSphere()
    {
        
    }

    Vector3 GetPosition() const;

    Vector3 GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const override
    {
        return intensity;
    }

private:

};

#endif