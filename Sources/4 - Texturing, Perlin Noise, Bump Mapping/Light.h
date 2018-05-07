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

    // lightPosition parameter is for area light since it changes its position with a random value and it would not be the same value if we randomize it again.
    virtual Vector3 GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const = 0;

    Vector3 position;
    Vector3 intensity;
};

#endif