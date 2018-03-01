/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <string>

#include "Math.h"

class Camera
{
public:
    Camera();
    ~Camera();

    Vector4 nearPlane;
    Vector3 gaze;
    Vector3 up;
    Vector3 right;
    Vector3 position;

    float nearDistance;
    unsigned int imageWidht;
    unsigned int imageHeight;

    std::string imageName;
private:
};

#endif