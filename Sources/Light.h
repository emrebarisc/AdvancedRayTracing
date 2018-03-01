#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Math.h"

class Light
{
public:
    Light();
    ~Light();

    Vector3 position;
    Vector3 intensity;
};

#endif