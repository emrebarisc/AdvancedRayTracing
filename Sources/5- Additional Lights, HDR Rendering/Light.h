#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Math.h"

class Light
{
public:
    Light()
    {

    }

    virtual ~Light()
    {

    }

    virtual Vector3 GetPosition() const
    {
        return position;
    }

    virtual Vector3 GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const = 0;

    virtual bool ShadowCheck(const Vector3& lightPosition, const Vector3& positionAt) const;

    Vector3 position;
    Vector3 intensity;
};

#endif